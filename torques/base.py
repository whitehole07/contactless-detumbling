__all__ = ["TorqueObject"]


class TorqueObject(object):
    def __new__(cls, torque_instance, _, *args, **kwargs):
        # Instantiate passed torque
        if type(torque_instance) is not list:
            torque_instance.__init__(*args, **kwargs)

        # Instantiate superclass
        return super().__new__(cls)

    def __init__(self, torque_instance, torque_eval, *args, **kwargs):
        # Save base torque instance
        self.base_torque = torque_instance

        # Save torque evaluation function
        self.eval_torque = torque_eval

    def __call__(self, t, y):
        # Execute evaluation function when instance of class is called
        return self.eval_torque(t, y)

    def __add__(self, other):
        # Create a new instance of the class
        # Sum the results of eval_torque from both torques
        return TorqueObject(
            [*self.de_list_able(self), *self.de_list_able(other)], lambda t, y: (self(t, y) + other(t, y))
        )

    @classmethod
    def de_list_able(cls, potential_list: list or object) -> list:
        """ Returns an unpack-able list in any case """
        return potential_list if type(potential_list) is list else [potential_list]
