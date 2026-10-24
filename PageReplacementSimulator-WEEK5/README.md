# Page Replacement Algorithms Simulator

## Project Overview
The Page Replacement Algorithms Simulator is a Python-based application that demonstrates how operating systems manage memory when page requests exceed the available page frames.

The application uses Streamlit to provide a graphical user interface and compares three page replacement algorithms: FIFO, LRU, and Optimal.

## Objectives
- Implement FIFO, LRU, and Optimal page replacement algorithms.
- Accept a page-reference string and the number of page frames.
- Display frame contents after each page request.
- Identify page hits and page faults.
- Calculate hit ratios and fault ratios.
- Compare algorithm performance using tables and charts.

## Technologies Used
- Python
- Streamlit
- Pandas
- Plotly
- Visual Studio Code
- Ubuntu / Linux

## Project Files
- `app.py` — Main Streamlit application and user interface.
- `fifo.py` — FIFO page replacement algorithm.
- `other_algorithms.py` — LRU and Optimal algorithms.
- `comparison.py` — Calculates statistics and compares algorithms.
- `requirements.txt` — Lists required Python packages.

## Features
- Enter page-reference strings using spaces or commas.
- Select the number of available page frames.
- Run FIFO, LRU, Optimal, or compare all algorithms.
- View step-by-step frame states.
- View page hits, page faults, hit ratios, and fault ratios.
- Compare results using tables and charts.
- Receive validation messages for invalid or empty inputs.

## Installation and Execution

### 1. Clone the repository
Open a terminal and navigate to the project folder.

### 2. Create a virtual environment
```bash
python3 -m venv .venv
```

### 3. Activate the virtual environment
```bash
source .venv/bin/activate
```

### 4. Install dependencies
```bash
python -m pip install -r requirements.txt
```

### 5. Run the application
```bash
python