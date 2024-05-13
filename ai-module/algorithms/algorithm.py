class Algorithm:
    def __init__(self, name):
        self.name = name
        self.predictor = None
        self.queue = []

    def get_name(self):
        return self.name

    def learning(self, training_set):
        pass

    def prediction(self, value):
        pass
