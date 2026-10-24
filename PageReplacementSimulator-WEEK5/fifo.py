
def simulate_fifo(reference_string, frame_count):
    """
    Simulate FIFO (First In, First Out) page replacement.

    Args:
        reference_string: List of page numbers.
        frame_count: Number of available memory frames.

    Returns:
        A list containing the state of frames after each page.
    """

    if frame_count <= 0:
        raise ValueError("Frame count must be greater than zero.")

    frames = []
    next_to_replace = 0
    hits = 0
    faults = 0
    steps = []

    for page in reference_string:
        if page in frames:
            hits += 1
            status = "Hit"
        else:
            faults += 1
            status = "Fault"

            if len(frames) < frame_count:
                frames.append(page)
            else:
                frames[next_to_replace] = page
                next_to_replace = (next_to_replace + 1) % frame_count

        steps.append({
            "Page": page,
            "Frames": frames.copy(),
            "Status": status,
            "Hits": hits,
            "Faults": faults
        })

    return steps

