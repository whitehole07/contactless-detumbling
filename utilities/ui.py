__all__ = ["LoadingBarWithTime", "print_loading_bar"]

import time


class LoadingBarWithTime:
    _instance = None

    def __new__(cls, title: str = ""):
        if cls._instance is None:
            cls._instance = super(LoadingBarWithTime, cls).__new__(cls)
            print(title, end="")
            cls._instance.start_time = time.time()
        return cls._instance

    def end(self):
        self._instance = None

    def update(self, current_element: int, total_elements: int):
        progress = (current_element / total_elements) * 100
        bar_length = 20
        num_blocks = int(round(bar_length * progress / 100))

        bar = ("[" + "=" * num_blocks +
               (">" if num_blocks != bar_length else "=") +
               " " * (bar_length - num_blocks) + "]")

        percentage = f" {progress:.2f}% ({current_element}/{total_elements})"

        # Compute ETA
        elapsed_time = time.time() - self.start_time
        if progress > 0:
            estimated_remaining_time = (elapsed_time / progress) * (100 - progress)
        else:
            estimated_remaining_time = 0

        remaining_time_minutes = estimated_remaining_time // 60
        remaining_time_seconds = estimated_remaining_time % 60

        remaining_time_str = f" ETA: {remaining_time_minutes:.0f}m {remaining_time_seconds:.0f}s"

        print(f"\r{bar} {percentage}{remaining_time_str}", end="", flush=True)

        if current_element == total_elements:
            print()  # Move to the next line after completion



def print_loading_bar(current, total):
    progress = (current / total) * 100
    bar_length = 20
    num_blocks = int(round(bar_length * progress / 100))

    bar = "[" + "=" * num_blocks + " " * (bar_length - num_blocks) + "]"
    percentage = f" {progress:.2f}% ({current}/{total})"

    print(f"\r{bar} {percentage}", end="", flush=True)

    if current == total:
        print()  # Move to the next line after completion
