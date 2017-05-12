import datetime

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session, url_for
from flask_session import Session
from passlib.apps import custom_app_context as pwd_context
from tempfile import mkdtemp

from helpers import *

# configure application
app = Flask(__name__)

# ensure responses aren't cached
if app.config["DEBUG"]:
    @app.after_request
    def after_request(response):
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Expires"] = 0
        response.headers["Pragma"] = "no-cache"
        return response

# custom filter
app.jinja_env.filters["usd"] = usd

# configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

@app.route("/")
@login_required
def index():
    total_assets = 0
    # get user information
    user = db.execute("SELECT * FROM users WHERE id = :user_id", user_id=session["user_id"])[0]
    total_assets += user['cash']
    
    # get stocks
    rows = db.execute("SELECT * FROM stocks WHERE user_id = :user_id", user_id=session["user_id"])
    
    # for each stock find current price
    for row in rows:
        row["current_price"] = lookup(row["symbol"])["price"]
        total_assets += row["current_price"] * row["quantity"]
        
    return render_template("index.html", user=user['username'], funds=usd(user['cash']), rows=rows, total_assets=usd(total_assets))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock."""
    
    if request.method == "POST":
        symbol = request.form.get("symbol")
    
        # reject if no symbol supplied by user
        if not symbol:
            flash("Stock symbol cannot be empty!", "error")
            return redirect(url_for("buy"))

        # reject if quantity not supplied
        if not request.form.get("quantity"):
            flash("Quantity must not be blank!", "error")
            return redirect(url_for("buy"))
        
        quantity = int(request.form.get("quantity"))

        # reject if quantity not a positive integer
        if not quantity > 0:
            flash("Quantity must be a positive number!", "error")
            return redirect(url_for("buy"))


        
        # find stock via lookup
        res = lookup(symbol)

        # reject if symbol not found by lookup
        if not res:
            flash("Sorry! Could not find stock {}. Please try again".format(symbol), "error")
            return redirect(url_for("buy"))

        # calculate total tranaction ammount
        total = int(quantity) * res["price"]

        # find user funds
        user_funds = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=session["user_id"])[0]["cash"]
        
        # reject if user does not have the desired funds.
        if user_funds < total:
            flash("Sorry, you do not have enough funds for this transaction", "error")
            return redirect(url_for("buy"))

        # !!Transaction Accepted at this point!!
        # Withdraw funds from user
        user_funds -= total
        db.execute("UPDATE users SET cash = :user_funds WHERE id = :user_id", user_funds=user_funds, user_id=session["user_id"])

        # find out if user already has stock row of this type.
        owned = db.execute("SELECT * FROM stocks WHERE symbol = :symbol AND user_id = :user_id", symbol=symbol, user_id=session["user_id"])
        print(owned)
        
        #   if so add to current row ;  else insert new row  to stock table
        if len(owned) > 0:
            db.execute("UPDATE stocks SET quantity = :new_quantity WHERE symbol = :symbol AND user_id = :user_id", 
                new_quantity=owned[0]["quantity"]+quantity, symbol=symbol, user_id=session["user_id"])
        else:
            db.execute("INSERT INTO 'stocks' ('id', 'user_id', 'symbol', 'quantity') VALUES (NULL, :user_id, :symbol, :quantity)", 
                user_id=session["user_id"], symbol=symbol, quantity=quantity)

        # add row to transaction table
        now = str(datetime.datetime.now())[:19]   
        db.execute("INSERT INTO 'transactions' ('id', 'user_id', 'symbol', 'quantity', 'unit_cost', 'time', 'type') VALUES (NULL, :user_id, :symbol, :quantity, :unit_cost, :time, :type)",
                    user_id=session["user_id"], symbol=symbol, quantity=quantity, unit_cost=res["price"], time=now, type="BUY")
        
        # flash success
        flash("SUCCESS! Purchased {} stocks of {}".format(quantity, symbol.upper()), "success")

        # return to index
        return redirect(url_for("index"))

    # return to buy.html
    return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions."""
    # get all transactions for current user.
    rows = db.execute("SELECT * FROM transactions WHERE user_id = :user_id", user_id=session["user_id"])
    
    # if no transactions flash and redirect to index.
    if not rows:
        flash("No history available for user.", "error")
        return redirect(url_for("index"))
    
    # render tmeplate with transactions
    return render_template("history.html", rows=rows)
    

@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in."""
    
    # make sure no user is surrently logged in
    if "user_id" in session:
        session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            flash("Username field cannot be blank", "error")
            return redirect(url_for("login"))

        # ensure password was submitted
        elif not request.form.get("password"):
            flash("Password field cannot be blank", "error")
            return redirect(url_for("login"))

        # query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))

        # ensure username exists and password is correct
        if len(rows) != 1 or not pwd_context.verify(request.form.get("password"), rows[0]["hash"]):
            flash("Invalid username and/or password provided", "error")
            return redirect(url_for("login"))

        # remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # redirect user to home page
        flash("Welcome back {}".format(rows[0]["username"]))
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out."""

    # forget any user_id
    session.clear()

    # redirect user to login form
    flash("Successfully logged out")
    return redirect(url_for("login"))


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    if request.method == "POST":
        
        # return apology if blank field.
        if not request.form.get("symbol"):
            flash("Stock field can not be blank", "error")
            return redirect(url_for("quote"))
        
        # get response from look up.
        res = lookup(request.form.get("symbol"))
        if res == None:
            flash("Sorry! Could not find stock {}. Please try again".format(request.form.get("symbol")), "error")
            return redirect(url_for("quote"))
            
        # use usd() helper function to format price.
        res["price"] = usd(res["price"])
        
        return render_template("quoted.html", res=res)
    return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user."""
    if request.method == "POST":
        
        # return apology if username field is blank
        if not request.form.get("username"):
            flash("Username required!", "error")
            return redirect(url_for("register"))
        
        # return apologf if password field is blank
        if not request.form.get("password"):
            flash("Password required!", "error")
            return redirect(url_for("register"))
            
        # return apology if password fields do not match 
        if not request.form.get("password") == request.form.get("password-repeat"):
            flash("Passwords don't match!", "error")
            return redirect(url_for("register"))
        
        # return apology if username is already in 
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))
        if rows:
            flash("Username already exists. Please choose another.", "error")
            return redirect(url_for("register"))
        
        # (if successful) insert new user into database
        db.execute("INSERT INTO 'users' ('id','username','hash') VALUES (NULL, :username, :hash)", 
            username=request.form.get("username"),
            hash=pwd_context.encrypt(request.form.get("password")))
    
        flash("Sucessfully registered. Please log in.", "success")
        return redirect(url_for("index"))
            
    return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock."""
    if request.method == "POST":
        symbol = request.form.get("symbol")
        quantity = int(request.form.get("quantity"))
        
        # reject if not a positive interger
        if not quantity > 0:
            flash("Quantity must be a positive number", "error")
            return redirect(url_for("sell"))
        
        # find users stock
        stock = db.execute("SELECT * FROM stocks WHERE user_id = :user_id AND symbol=:symbol", 
            user_id=session["user_id"],
            symbol=symbol)[0]
        
        # if user is attempting to sell more stock than they posses, return apology.
        if quantity > stock["quantity"]:
            flash("Sorry, you don't have that many to sell : Currently owned: {}".format(stock["quantity"]), "error")
            return redirect(url_for("sell"))
        
        # alter database, flash mesage and redirect.
        res = lookup(symbol)
        sale_funds = quantity * res["price"]
        
        # update users cash
        current_funds = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=session["user_id"])[0]["cash"]
        current_funds += sale_funds
        
        db.execute("UPDATE users SET cash = :current_funds WHERE id = :user_id", 
            current_funds=current_funds,
            user_id=session["user_id"])
        
        # sell stock. if user is selling all the stock they own. delete row from stock table. else update row
        if quantity == stock["quantity"]:
            db.execute("DELETE FROM stocks WHERE user_id = :user_id AND symbol=:symbol", 
                user_id=session["user_id"],
                symbol=symbol)
        else:
            new_quantity = stock["quantity"] - quantity
            db.execute("UPDATE stocks SET quantity = :new_quantity WHERE user_id = :user_id AND symbol = :symbol", 
                new_quantity=new_quantity,
                user_id=session["user_id"],
                symbol=symbol)
        
        # add row to transaction table
        now = str(datetime.datetime.now())[:19]   
        db.execute("INSERT INTO 'transactions' ('id', 'user_id', 'symbol', 'quantity', 'unit_cost', 'time', 'type') VALUES (NULL, :user_id, :symbol, :quantity, :unit_cost, :time, :type)",
                    user_id=session["user_id"], symbol=symbol, quantity=quantity, unit_cost=(res["price"]), time=now, type="SELL")
        # flash success
        
        flash("SUCCESS! Sold {} stocks of {} for {}".format(quantity, symbol.upper(), usd(sale_funds)), "success")
        
        return redirect(url_for("index"))
    
    
    # populate select box with current stocks owned. 
    user_stocks = db.execute("SELECT symbol FROM stocks WHERE user_id = :user_id", user_id=session["user_id"])
    
    if not len(user_stocks) > 0:
        flash("I'm sorry, but you have no stocks to sell!", "error")
        return redirect(url_for("index"))

    return render_template("sell.html", user_stocks=user_stocks)


@app.route("/add", methods=["GET", "POST"])
@login_required
def add():
    """Add funds to users account."""
    
    # get data to populate html    
    user = db.execute("SELECT * FROM users WHERE id = :user_id", user_id=session["user_id"])[0]
    user_cash = user["cash"]
    username = user["username"]

    if request.method == "POST":
        funds = int(request.form.get("funds"))
        
        # run some checks
        if not funds:
            flash("Funds must not be blank.", "error")
            return redirect(url_for("add"))
        
        if not funds > 0:
            flash("Funds must be a positive number.", "error")
            return redirect(url_for("add"))
        
        # Add funds and update DB
        new_funds = funds + user_cash
        db.execute("UPDATE users SET cash = :new_funds WHERE id = :user_id", new_funds=new_funds ,user_id=session["user_id"])
        
        flash("Added funds to your account!", "success")
        return redirect(url_for('index'))

    return render_template("add.html", user=username, cash=user_cash)