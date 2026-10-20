
const allocation = [
    [0, 1, 0],
    [2, 0, 0],
    [3, 0, 2],
    [2, 1, 1],
    [0, 0, 2]
];

const maximum = [
    [7, 5, 3],
    [3, 2, 2],
    [9, 0, 2],
    [2, 2, 2],
    [4, 3, 3]
];

const available = [3, 3, 2];

const need = maximum.map((row, i) =>
    row.map((value, j) => value - allocation[i][j])
);

function displayMatrix(elementId, matrix) {
    const container = document.getElementById(elementId);

    let html = `
        <table>
            <thead>
                <tr>
                    <th>Process</th>
                    <th>A</th>
                    <th>B</th>
                    <th>C</th>
                </tr>
            </thead>
            <tbody>
    `;

    matrix.forEach((row, i) => {
        html += `
            <tr>
                <td>P${i}</td>
                ${row.map(value => `<td>${value}</td>`).join("")}
            </tr>
        `;
    });

    html += "</tbody></table>";
    container.innerHTML = html;
}

displayMatrix("allocation-matrix", allocation);
displayMatrix("maximum-matrix", maximum);
displayMatrix("need-matrix", need);

function addLog(message) {
    const log = document.getElementById("activity-log");
    log.textContent += message + "\n";
}

addLog("Deadlock Simulation System initialized.");
addLog("5 processes and 3 resource types loaded.");
addLog("Allocation, Maximum and Need matrices displayed.");
