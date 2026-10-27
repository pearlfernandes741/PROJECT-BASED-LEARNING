def calculate_statistics(steps):
    """Calculate hits, faults, and ratios for an algorithm."""

    if not steps:
        return {
            "Hits": 0,
            "Page Faults": 0,
            "Hit Ratio": 0.0,
            "Fault Ratio": 0.0
        }

    total_pages = len(steps)
    hits = steps[-1]["Hits"]
    faults = steps[-1]["Faults"]

    return {
        "Hits": hits,
        "Page Faults": faults,
        "Hit Ratio": round((hits / total_pages) * 100, 2),
        "Fault Ratio": round((faults / total_pages) * 100, 2)
    }


def build_comparison(results):
    """Create a comparison summary for multiple algorithms."""

    comparison = {}

    for algorithm_name, steps in results.items():
        comparison[algorithm_name] = calculate_statistics(steps)

    return comparison


def find_best_algorithm(comparison):
    """Find the algorithm with the fewest page faults."""

    if not comparison:
        return "No results available"

    return min(
        comparison,
        key=lambda name: comparison[name]["Page Faults"]
    )

