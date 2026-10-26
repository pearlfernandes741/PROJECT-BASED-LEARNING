import streamlit as st
import pandas as pd
import plotly.express as px

from fifo import simulate_fifo
from other_algorithms import simulate_lru, simulate_optimal
from comparison import build_comparison, find_best_algorithm


# Page configuration
st.set_page_config(
    page_title="Page Replacement Simulator",
    layout="wide"
)

# Title and description
st.title("Page Replacement Simulator")
st.write(
    "Simulate and compare FIFO, LRU, and Optimal page replacement algorithms."
)

st.divider()

# Sidebar inputs
st.sidebar.header("Simulation Settings")

reference_input = st.sidebar.text_input(
    "Page Reference String",
    value="7 0 1 2 0 3 0 4 2 3",
    help="Enter page numbers separated by spaces or commas."
)

frame_count = st.sidebar.number_input(
    "Number of Frames",
    min_value=1,
    max_value=20,
    value=3,
    step=1
)

algorithm_choice = st.sidebar.selectbox(
    "Choose Algorithm",
    ["Compare All", "FIFO", "LRU", "Optimal"]
)

run_button = st.sidebar.button(
    "Run Simulation",
    type="primary",
    use_container_width=True
)

# Main simulation
if run_button:
    try:
        # Accept spaces or commas between page numbers
        cleaned_input = reference_input.replace(",", " ").split() if " " in reference_input or "," in reference_input else list(reference_input)

        if not cleaned_input:
            st.error("Please enter a page-reference string.")
            st.stop()

        if any(not item.isdigit() for item in cleaned_input):
            st.error("Enter only non-negative whole numbers.")
            st.stop()

        pages = [int(item) for item in cleaned_input]
        frame_count = int(frame_count)

        # Run all algorithms
        all_results = {
            "FIFO": simulate_fifo(pages, frame_count),
            "LRU": simulate_lru(pages, frame_count),
            "Optimal": simulate_optimal(pages, frame_count)
        }

        # Select requested algorithms
        if algorithm_choice == "Compare All":
            selected_results = all_results
        else:
            selected_results = {
                algorithm_choice: all_results[algorithm_choice]
            }

        # Summary statistics
        summary = build_comparison(selected_results)

        st.subheader("Simulation Results")

        # Display statistics for each selected algorithm
        for name, stats in summary.items():
            st.markdown(f"### {name}")

            col1, col2, col3, col4 = st.columns(4)

            col1.metric("Page Hits", stats["Hits"])
            col2.metric("Page Faults", stats["Page Faults"])
            col3.metric("Hit Ratio", f'{stats["Hit Ratio"]}%')
            col4.metric("Fault Ratio", f'{stats["Fault Ratio"]}%')

            # Display step-by-step frame states
            st.markdown(f"#### {name}: Step-by-Step Execution")

            step_rows = []

            for step_number, step in enumerate(
                selected_results[name], start=1
            ):
                row = {
                    "Step": step_number,
                    "Page": step["Page"]
                }

                for frame_index in range(frame_count):
                    if frame_index < len(step["Frames"]):
                        row[f"Frame {frame_index + 1}"] = (
                            step["Frames"][frame_index]
                        )
                    else:
                        row[f"Frame {frame_index + 1}"] = "-"

                row["Status"] = step["Status"]
                row["Hits"] = step["Hits"]
                row["Faults"] = step["Faults"]

                step_rows.append(row)

            st.dataframe(
                pd.DataFrame(step_rows),
                use_container_width=True,
                hide_index=True
            )

            st.divider()

        # Comparison table and charts
        if algorithm_choice == "Compare All":
            st.subheader("Algorithm Comparison")

            comparison_df = pd.DataFrame(summary).T
            comparison_df.index.name = "Algorithm"

            st.dataframe(
                comparison_df,
                use_container_width=True
            )

            chart_data = comparison_df.reset_index()

            st.markdown("#### Page Fault Comparison")

            fault_chart = px.bar(
                chart_data,
                x="Algorithm",
                y="Page Faults",
                color="Algorithm",
                title="Total Page Faults by Algorithm"
            )
            st.plotly_chart(fault_chart, use_container_width=True)

            st.markdown("#### Hit Ratio Comparison")

            hit_chart = px.bar(
                chart_data,
                x="Algorithm",
                y="Hit Ratio",
                color="Algorithm",
                title="Hit Ratio (%) by Algorithm"
            )
            st.plotly_chart(hit_chart, use_container_width=True)

            best_algorithm = find_best_algorithm(summary)

            st.success(
                f"Best result for this reference string: {best_algorithm}"
                f" — it has the fewest page faults."
            )

        st.caption(
            "Note: Optimal uses future page references to choose which "
            "page to replace. It is a theoretical benchmark."
        )

    except ValueError as error:
        st.error(f"Invalid input: {error}")

else:
    st.info(
        "Enter your page-reference string, choose the number of frames, "
        "select an algorithm, and click Run Simulation."
    )

    st.markdown("### How to use")
    st.write("1. Enter page numbers separated by spaces or commas.")
    st.write("2. Select the number of memory frames.")
    st.write("3. Choose FIFO, LRU, Optimal, or Compare All.")
    st.write("4. Click Run Simulation to view the results.")