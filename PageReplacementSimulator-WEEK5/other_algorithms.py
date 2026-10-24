
def simulate_lru(reference_string, frame_count):
    """Simulate Least Recently Used (LRU) page replacement."""

    if frame_count <= 0:
        raise ValueError("Frame count must be greater than zero.")

    frames = []
    last_used = {}
    hits = 0
    faults = 0
    steps = []

    for index, page in enumerate(reference_string):
        if page in frames:
            hits += 1
            status = "Hit"
        else:
            faults += 1
            status = "Fault"

            if len(frames) < frame_count:
                frames.append(page)
            else:
                lru_page = min(frames, key=lambda p: last_used[p])
                replace_index = frames.index(lru_page)
                frames[replace_index] = page

        last_used[page] = index

        steps.append({
            "Page": page,
            "Frames": frames.copy(),
            "Status": status,
            "Hits": hits,
            "Faults": faults
        })

    return steps


def simulate_optimal(reference_string, frame_count):
    """Simulate Optimal page replacement."""

    if frame_count <= 0:
        raise ValueError("Frame count must be greater than zero.")

    frames = []
    hits = 0
    faults = 0
    steps = []

    for index, page in enumerate(reference_string):
        if page in frames:
            hits += 1
            status = "Hit"
        else:
            faults += 1
            status = "Fault"

            if len(frames) < frame_count:
                frames.append(page)
            else:
                future_pages = reference_string[index + 1:]
                future_positions = {}

                for frame_page in frames:
                    if frame_page in future_pages:
                        future_positions[frame_page] = future_pages.index(frame_page)
                    else:
                        future_positions[frame_page] = float("inf")

                page_to_replace = max(
                    frames,
                    key=lambda p: future_positions[p]
                )

                replace_index = frames.index(page_to_replace)
                frames[replace_index] = page

        steps.append({
            "Page": page,
            "Frames": frames.copy(),
            "Status": status,
            "Hits": hits,
            "Faults": faults
        })

    return steps
