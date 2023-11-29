__all__ = ["TorqueObject"]


class TorqueObject(object):
    def __init__(self, eval_funct):
        # Save torque evaluation function
        self.eval_torque = eval_funct

    def __call__(self, t, y):
        # Execute evaluation function when instance of class is called
        return self.eval_torque(t, y)

    def __add__(self, other):
        # Create a new instance of the class
        # Sum the results of eval_torque from both torques
        return TorqueObject(
            lambda t, y: (self(t, y) + other(t, y))
        )
