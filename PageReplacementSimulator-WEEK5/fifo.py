# Define a function to simulate FIFO page replacement
# reference_string contains the sequence of requested pages
# frame_count specifies the number of available memory frames
def simulate_fifo(reference_string, frame_count):

    """
    Simulate FIFO (First In, First Out) page replacement.

    Args:
        reference_string: List of page numbers.
        frame_count: Number of available memory frames.

    Returns:
        A list containing the state of frames after each page.
    """

    # Check whether the number of frames is valid
    if frame_count <= 0:

        # Raise an error if the frame count is zero or negative
        raise ValueError("Frame count must be greater than zero.")

    # Create an empty list to store pages currently in memory
    frames = []

    # Track the index of the next page to be replaced
    next_to_replace = 0

    # Initialize the number of page hits to zero
    hits = 0

    # Initialize the number of page faults to zero
    faults = 0

    # Create an empty list to store results after each page request
    steps = []

    # Loop through each page in the reference string
    for page in reference_string:

        # Check whether the requested page is already in memory
        if page in frames:

            # Increase the hit count because the page is present
            hits += 1

            # Mark the current request as a page hit
            status = "Hit"

        else:

            # Increase the fault count because the page is not in memory
            faults += 1

            # Mark the current request as a page fault
            status = "Fault"

            # Check whether there is an empty memory frame
            if len(frames) < frame_count:

                # Add the requested page to an available frame
                frames.append(page)

            else:

                # Replace the oldest page according to FIFO order
                frames[next_to_replace] = page

                # Move the replacement pointer to the next frame
                # Modulo (%) makes the pointer return to zero
                # after reaching the last frame
                next_to_replace = (next_to_replace + 1) % frame_count

        # Store the results of the current page request
        steps.append({

            # Store the page number that was requested
            "Page": page,

            # Store a copy of the current memory frames
            # copy() prevents previous results from changing later
            "Frames": frames.copy(),

            # Store whether the request was a Hit or Fault
            "Status": status,

            # Store the total number of hits so far
            "Hits": hits,

            # Store the total number of faults so far
            "Faults": faults
        })

    # Return the list containing all page replacement steps
    return steps