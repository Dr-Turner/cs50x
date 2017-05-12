import nltk

class Analyzer():
    """Implements sentiment analysis."""

    def __init__(self, positives, negatives):
        """Initialize Analyzer."""
        self.positives = self._get_words(positives)
        self.negatives = self._get_words(negatives)


    @staticmethod
    def _get_words(filename):
        """Grabs suitable words from a text file and returns a list"""

        words = list()
        with open(filename) as f:
            for line in f.readlines():
                if not line.startswith(';') or line.startswith(' '):
                    words.append(line.strip().lower())
        return words

    def analyze(self, text):
        """Analyze text for sentiment, returning its score."""

        # chop string up into list of words
        tokenizer = nltk.tokenize
        words = tokenizer.casual_tokenize(text.lower())

        # analyse each word in text and adjust score
        score = 0
        for word in words:
            if word in self.positives:
                score += 1
            if word in self.negatives:
                score -= 1

        return score
