#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algorithms.h"
#include "simulation.h"
#include "graph.h"

static GtkWidget *status_label, *output_view, *matrix_grid, *graph_box;
static GtkWidget *process_spin, *resource_spin;
static GtkWidget *request_process, *release_process, *recovery_process;
static GtkWidget *request_vector, *release_vector;
static GtkWidget *alloc_entry[MAX_PROCESSES][MAX_RESOURCES];
static GtkWidget *max_entry[MAX_PROCESSES][MAX_RESOURCES];
static GtkWidget *available_entry[MAX_RESOURCES];

static void rebuild_matrix_editor(void);

static void show_output(const char *message)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));
    gtk_text_buffer_set_text(buffer, message, -1);
}

static void set_status(const char *message)
{
    gtk_label_set_text(GTK_LABEL(status_label), message);
}

static void append_output(const char *message)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, message, -1);
    gtk_text_buffer_insert(buffer, &end, "\n", -1);
}

static GtkWidget *section_label(const char *text)
{
    GtkWidget *label = gtk_label_new(NULL);
    char markup[256];
    snprintf(markup, sizeof(markup), "<b>%s</b>", text);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    return label;
}

static GtkWidget *button(const char *text, GCallback callback)
{
    GtkWidget *b = gtk_button_new_with_label(text);
    g_signal_connect(b, "clicked", callback, NULL);
    return b;
}

static GtkWidget *scroll(GtkWidget *child)
{
    GtkWidget *s = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(s), GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(s), child);
    return s;
}

static void update_process_combos(void)
{
    GtkWidget *combos[] = { request_process, release_process, recovery_process };
    for (unsigned int c = 0; c < sizeof(combos) / sizeof(combos[0]); c++) {
        gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(combos[c]));
        for (int i = 0; i < process_count; i++) {
            char label[16];
            snprintf(label, sizeof(label), "P%d", i);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combos[c]), label);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(combos[c]), 0);
    }
}

static void refresh_status(void)
{
    int safe_sequence[MAX_PROCESSES] = {0};
    int deadlocked[MAX_PROCESSES] = {0};
    int safe = banker_algorithm(safe_sequence);
    detect_deadlock(deadlocked);
    int count = 0;
    for (int i = 0; i < process_count; i++)
        if (deadlocked[i]) count++;

    if (count > 0)
        set_status("SYSTEM STATUS: DEADLOCKED — inspect the deadlocked process list");
    else if (safe)
        set_status("SYSTEM STATUS: SAFE");
    else
        set_status("SYSTEM STATUS: UNSAFE");
}

static void refresh_graph(void)
{
    GList *children = gtk_container_get_children(GTK_CONTAINER(graph_box));
    for (GList *item = children; item != NULL; item = item->next)
        gtk_widget_destroy(GTK_WIDGET(item->data));
    g_list_free(children);

    GtkWidget *graph = create_resource_allocation_graph();
    if (graph)
        gtk_box_pack_start(GTK_BOX(graph_box), graph, TRUE, TRUE, 0);
    gtk_widget_show_all(graph_box);
}

static void refresh_all(void)
{
    calculate_need();
    update_process_combos();
    refresh_status();
    refresh_graph();
}

static void show_matrices(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    calculate_need();
    char out[8192];
    size_t used = 0;
#define ADD(...) do { if (used < sizeof(out)) used += (size_t)snprintf(out + used, sizeof(out) - used, __VA_ARGS__); } while (0)
    ADD("AVAILABLE RESOURCE VECTOR\n");
    for (int j = 0; j < resource_count; j++) ADD("%c=%d  ", 'A' + j, available[j]);
    ADD("\n\nALLOCATION MATRIX\n");
    for (int i = 0; i < process_count; i++) { ADD("P%d: ", i); for (int j = 0; j < resource_count; j++) ADD("%d ", allocation[i][j]); ADD("\n"); }
    ADD("\nMAXIMUM REQUIREMENT MATRIX\n");
    for (int i = 0; i < process_count; i++) { ADD("P%d: ", i); for (int j = 0; j < resource_count; j++) ADD("%d ", maximum[i][j]); ADD("\n"); }
    ADD("\nNEED MATRIX (Maximum - Allocation)\n");
    for (int i = 0; i < process_count; i++) { ADD("P%d: ", i); for (int j = 0; j < resource_count; j++) ADD("%d ", need[i][j]); ADD("\n"); }
#undef ADD
    show_output(out);
}

static void run_banker(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    int sequence[MAX_PROCESSES] = {0};
    if (banker_algorithm(sequence)) {
        char out[1024];
        size_t used = (size_t)snprintf(out, sizeof(out), "BANKER'S ALGORITHM: SAFE\nSafe sequence: ");
        for (int i = 0; i < process_count; i++)
            used += (size_t)snprintf(out + used, sizeof(out) - used, "P%d%s", sequence[i], i == process_count - 1 ? "" : " -> ");
        show_output(out);
        set_status("SYSTEM STATUS: SAFE");
    } else {
        show_output("BANKER'S ALGORITHM: UNSAFE\nThe system cannot guarantee that every active process can finish.");
        set_status("SYSTEM STATUS: UNSAFE");
    }
}

static void detect_deadlock_clicked(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    int deadlocked[MAX_PROCESSES] = {0};
    detect_deadlock(deadlocked);
    char out[1024] = "DEADLOCK DETECTION RESULT\n";
    size_t used = strlen(out);
    int count = 0;
    for (int i = 0; i < process_count; i++) {
        if (deadlocked[i]) {
            used += (size_t)snprintf(out + used, sizeof(out) - used, "P%d is marked as deadlocked.\n", i);
            count++;
        }
    }
    if (!count) snprintf(out + used, sizeof(out) - used, "No deadlocked processes were identified.");
    show_output(out);
    refresh_status();
}

static gboolean parse_vector(const char *text, int vector[], int expected)
{
    char copy[256];
    if (strlen(text) >= sizeof(copy)) return FALSE;
    strcpy(copy, text);
    int count = 0;
    char *token = strtok(copy, ", ");
    while (token) {
        char *end = NULL;
        long value = strtol(token, &end, 10);
        if (end == token || *end != '\0' || value < 0 || value > 100000 || count >= expected)
            return FALSE;
        vector[count++] = (int)value;
        token = strtok(NULL, ", ");
    }
    return count == expected;
}

static void request_resources_clicked(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    int process = gtk_combo_box_get_active(GTK_COMBO_BOX(request_process));
    int request[MAX_RESOURCES] = {0};
    if (process < 0 || !parse_vector(gtk_entry_get_text(GTK_ENTRY(request_vector)), request, resource_count)) {
        show_output("Invalid request vector. Enter one non-negative integer per resource, separated by commas, e.g. 1, 0, 0.");
        return;
    }
    if (request_resources(process, request))
        show_output("REQUEST GRANTED. The simulation accepted the request.");
    else
        show_output("REQUEST DENIED. Check available resources, remaining Need, and system safety.");
    refresh_all();
}

static void release_resources_clicked(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    int process = gtk_combo_box_get_active(GTK_COMBO_BOX(release_process));
    int release[MAX_RESOURCES] = {0};
    if (process < 0 || !parse_vector(gtk_entry_get_text(GTK_ENTRY(release_vector)), release, resource_count)) {
        show_output("Invalid release vector. Enter one non-negative integer per resource, separated by commas.");
        return;
    }
    if (release_resources(process, release))
        show_output("Resource release completed.");
    else
        show_output("Release rejected. A process cannot release more than it currently holds.");
    refresh_all();
}

static void recover_process_clicked(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    int process = gtk_combo_box_get_active(GTK_COMBO_BOX(recovery_process));
    if (process < 0) { show_output("Select a process to recover."); return; }
    if (recover_process(process)) {
        char out[160];
        snprintf(out, sizeof(out), "Recovery completed for P%d.", process);
        show_output(out);
    } else {
        show_output("Recovery failed for the selected process.");
    }
    refresh_all();
}

static gboolean read_nonnegative(GtkWidget *entry, int *value)
{
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry));
    char *end = NULL;
    long number = strtol(text, &end, 10);
    if (end == text || *end != '\0' || number < 0 || number > 100000) return FALSE;
    *value = (int)number;
    return TRUE;
}

static void apply_matrix_edits(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    int new_alloc[MAX_PROCESSES][MAX_RESOURCES] = {{0}};
    int new_max[MAX_PROCESSES][MAX_RESOURCES] = {{0}};
    int new_available[MAX_RESOURCES] = {0};

    for (int j = 0; j < resource_count; j++) {
        if (!read_nonnegative(available_entry[j], &new_available[j])) {
            show_output("Available values must be non-negative whole numbers."); return;
        }
    }
    for (int i = 0; i < process_count; i++) {
        for (int j = 0; j < resource_count; j++) {
            if (!read_nonnegative(alloc_entry[i][j], &new_alloc[i][j]) ||
                !read_nonnegative(max_entry[i][j], &new_max[i][j]) ||
                new_alloc[i][j] > new_max[i][j]) {
                show_output("Invalid matrix. Use non-negative integers and make sure Allocation is not greater than Maximum."); return;
            }
        }
    }
    for (int j = 0; j < resource_count; j++) {
        int total_allocated = 0;
        for (int i = 0; i < process_count; i++) total_allocated += new_alloc[i][j];
        if (total_allocated < 0) { show_output("Invalid allocation total."); return; }
    }
    for (int i = 0; i < process_count; i++) {
        for (int j = 0; j < resource_count; j++) {
            allocation[i][j] = new_alloc[i][j];
            maximum[i][j] = new_max[i][j];
        }
    }
    for (int j = 0; j < resource_count; j++) available[j] = new_available[j];
    calculate_need();
    show_output("Matrix edits applied. Need matrix and system status have been updated.");
    refresh_all();
}

static void rebuild_matrix_editor(void)
{
    GList *children = gtk_container_get_children(GTK_CONTAINER(matrix_grid));
    for (GList *item = children; item; item = item->next)
        gtk_widget_destroy(GTK_WIDGET(item->data));
    g_list_free(children);

    gtk_grid_set_row_spacing(GTK_GRID(matrix_grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(matrix_grid), 8);
    gtk_grid_attach(GTK_GRID(matrix_grid), section_label("Process"), 0, 0, 1, 1);
    for (int j = 0; j < resource_count; j++) {
        char label[32]; snprintf(label, sizeof(label), "Alloc %c", 'A' + j);
        gtk_grid_attach(GTK_GRID(matrix_grid), section_label(label), 1 + j, 0, 1, 1);
    }
    for (int j = 0; j < resource_count; j++) {
        char label[32]; snprintf(label, sizeof(label), "Max %c", 'A' + j);
        gtk_grid_attach(GTK_GRID(matrix_grid), section_label(label), 1 + resource_count + j, 0, 1, 1);
    }
    for (int i = 0; i < process_count; i++) {
        char p[16]; snprintf(p, sizeof(p), "P%d", i);
        gtk_grid_attach(GTK_GRID(matrix_grid), gtk_label_new(p), 0, i + 1, 1, 1);
        for (int j = 0; j < resource_count; j++) {
            char value[24];
            alloc_entry[i][j] = gtk_entry_new();
            snprintf(value, sizeof(value), "%d", allocation[i][j]);
            gtk_entry_set_text(GTK_ENTRY(alloc_entry[i][j]), value);
            gtk_entry_set_width_chars(GTK_ENTRY(alloc_entry[i][j]), 4);
            gtk_grid_attach(GTK_GRID(matrix_grid), alloc_entry[i][j], 1 + j, i + 1, 1, 1);

            max_entry[i][j] = gtk_entry_new();
            snprintf(value, sizeof(value), "%d", maximum[i][j]);
            gtk_entry_set_text(GTK_ENTRY(max_entry[i][j]), value);
            gtk_entry_set_width_chars(GTK_ENTRY(max_entry[i][j]), 4);
            gtk_grid_attach(GTK_GRID(matrix_grid), max_entry[i][j], 1 + resource_count + j, i + 1, 1, 1);
        }
    }
    int row = process_count + 2;
    gtk_grid_attach(GTK_GRID(matrix_grid), section_label("Available"), 0, row, 1, 1);
    for (int j = 0; j < resource_count; j++) {
        char value[24];
        available_entry[j] = gtk_entry_new();
        snprintf(value, sizeof(value), "%d", available[j]);
        gtk_entry_set_text(GTK_ENTRY(available_entry[j]), value);
        gtk_entry_set_width_chars(GTK_ENTRY(available_entry[j]), 4);
        gtk_grid_attach(GTK_GRID(matrix_grid), available_entry[j], 1 + j, row, 1, 1);
    }
    gtk_widget_show_all(matrix_grid);
}

static void create_system(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    int p = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(process_spin));
    int r = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(resource_spin));
    process_count = p;
    resource_count = r;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        for (int j = 0; j < MAX_RESOURCES; j++) {
            allocation[i][j] = 0;
            maximum[i][j] = (i < p && j < r) ? 1 : 0;
            need[i][j] = 0;
        }
    }
    for (int j = 0; j < MAX_RESOURCES; j++) available[j] = j < r ? 3 : 0;
    calculate_need();
    rebuild_matrix_editor();
    refresh_all();
    show_output("Created a new system. Edit the Allocation, Maximum, and Available fields, then click Apply matrix edits.");
}

static void load_classic_example(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    process_count = 5; resource_count = 3;
    const int a[5][3] = {{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}};
    const int m[5][3] = {{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}};
    const int v[3] = {3,3,2};
    for (int i = 0; i < MAX_PROCESSES; i++) {
        for (int j = 0; j < MAX_RESOURCES; j++) {
            allocation[i][j] = (i < 5 && j < 3) ? a[i][j] : 0;
            maximum[i][j] = (i < 5 && j < 3) ? m[i][j] : 0;
            need[i][j] = 0;
        }
    }
    for (int j = 0; j < MAX_RESOURCES; j++) available[j] = j < 3 ? v[j] : 0;
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(process_spin), 5);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(resource_spin), 3);
    calculate_need();
    rebuild_matrix_editor();
    refresh_all();
    show_output("Loaded classic Banker’s Algorithm example. Expected safe sequence: P1 -> P3 -> P4 -> P0 -> P2.");
}

static void display_need(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    calculate_need();
    char out[4096]; size_t used = 0;
    used += (size_t)snprintf(out + used, sizeof(out) - used, "NEED MATRIX (Maximum - Allocation)\n");
    for (int i = 0; i < process_count; i++) {
        used += (size_t)snprintf(out + used, sizeof(out) - used, "P%d: ", i);
        for (int j = 0; j < resource_count; j++)
            used += (size_t)snprintf(out + used, sizeof(out) - used, "%d ", need[i][j]);
        used += (size_t)snprintf(out + used, sizeof(out) - used, "\n");
    }
    show_output(out);
}

static void show_rag(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    refresh_graph();
    show_output("Resource Allocation Graph refreshed. Solid teal arrows represent current allocations; inspect the graph tab for the current state.");
}

static void show_system_status(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    int seq[MAX_PROCESSES] = {0}, deadlocked[MAX_PROCESSES] = {0};
    int safe = banker_algorithm(seq);
    detect_deadlock(deadlocked);
    char out[2048]; size_t used = 0;
    used += (size_t)snprintf(out + used, sizeof(out) - used, "SYSTEM STATUS\nAvailable: ");
    for (int j = 0; j < resource_count; j++)
        used += (size_t)snprintf(out + used, sizeof(out) - used, "%c=%d ", 'A' + j, available[j]);
    used += (size_t)snprintf(out + used, sizeof(out) - used, "\nBanker's Algorithm: %s\n", safe ? "SAFE" : "UNSAFE");
    int count = 0;
    for (int i = 0; i < process_count; i++) if (deadlocked[i]) {
        used += (size_t)snprintf(out + used, sizeof(out) - used, "Deadlocked process: P%d\n", i);
        count++;
    }
    if (!count) used += (size_t)snprintf(out + used, sizeof(out) - used, "Deadlocked processes: none reported\n");
    if (safe) {
        used += (size_t)snprintf(out + used, sizeof(out) - used, "Safe sequence: ");
        for (int i = 0; i < process_count; i++)
            used += (size_t)snprintf(out + used, sizeof(out) - used, "P%d%s", seq[i], i == process_count - 1 ? "" : " -> ");
    }
    show_output(out);
    refresh_status();
}

static void activate(GtkApplication *app, gpointer data)
{
    GtkWidget *window;
    (void)data;
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Deadlock Simulation System — Week 4");
    gtk_window_set_default_size(GTK_WINDOW(window), 1180, 820);
    gtk_container_set_border_width(GTK_CONTAINER(window), 14);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), root);
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='xx-large' weight='bold'>Deadlock Simulation System</span>\n<span size='small'>Operating Systems Lab · Interactive Week 4 Project</span>");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0f);
    gtk_box_pack_start(GTK_BOX(root), title, FALSE, FALSE, 0);

    status_label = gtk_label_new("SYSTEM STATUS");
    gtk_label_set_xalign(GTK_LABEL(status_label), 0.0f);
    gtk_box_pack_start(GTK_BOX(root), status_label, FALSE, FALSE, 0);

    GtkWidget *notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(root), notebook, TRUE, TRUE, 0);

    GtkWidget *dashboard = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(dashboard), 12);
    GtkWidget *dimrow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_pack_start(GTK_BOX(dimrow), gtk_label_new("Processes:"), FALSE, FALSE, 0);
    process_spin = gtk_spin_button_new_with_range(1, MAX_PROCESSES, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(process_spin), process_count);
    gtk_box_pack_start(GTK_BOX(dimrow), process_spin, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dimrow), gtk_label_new("Resource types:"), FALSE, FALSE, 0);
    resource_spin = gtk_spin_button_new_with_range(1, MAX_RESOURCES, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(resource_spin), resource_count);
    gtk_box_pack_start(GTK_BOX(dimrow), resource_spin, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dimrow), button("1-2. Create processes / resources", G_CALLBACK(create_system)), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dimrow), button("Load classic example", G_CALLBACK(load_classic_example)), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dashboard), dimrow, FALSE, FALSE, 0);

    GtkWidget *actions = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(actions), 6);
    gtk_grid_set_column_spacing(GTK_GRID(actions), 6);
    const char *labels[] = {"3-4. Edit matrices", "5. Display Need", "6. Display RAG", "8. Banker’s Algorithm", "9. Detect deadlock", "11. System status"};
    GCallback cbs[] = {G_CALLBACK(show_matrices), G_CALLBACK(display_need), G_CALLBACK(show_rag), G_CALLBACK(run_banker), G_CALLBACK(detect_deadlock_clicked), G_CALLBACK(show_system_status)};
    for (int i = 0; i < 6; i++) {
        GtkWidget *b = button(labels[i], cbs[i]);
        gtk_widget_set_hexpand(b, TRUE);
        gtk_grid_attach(GTK_GRID(actions), b, i % 3, i / 3, 1, 1);
    }
    gtk_box_pack_start(GTK_BOX(dashboard), actions, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dashboard), section_label("Results / algorithm output"), FALSE, FALSE, 0);
    output_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(output_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(output_view), TRUE);
    gtk_widget_set_size_request(output_view, -1, 180);
    gtk_box_pack_start(GTK_BOX(dashboard), scroll(output_view), TRUE, TRUE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll(dashboard), gtk_label_new("Dashboard"));

    GtkWidget *matrix_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(matrix_page), 12);
    gtk_box_pack_start(GTK_BOX(matrix_page), section_label("Editable Allocation, Maximum and Available values"), FALSE, FALSE, 0);
    matrix_grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(matrix_page), matrix_grid, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(matrix_page), button("Apply matrix edits", G_CALLBACK(apply_matrix_edits)), FALSE, FALSE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll(matrix_page), gtk_label_new("Matrices"));

    GtkWidget *graph_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(graph_page), 12);
    gtk_box_pack_start(GTK_BOX(graph_page), section_label("Resource Allocation Graph"), FALSE, FALSE, 0);
    graph_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(graph_page), graph_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(graph_page), button("Refresh graph", G_CALLBACK(show_rag)), FALSE, FALSE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll(graph_page), gtk_label_new("Resource Allocation Graph"));

    GtkWidget *sim_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(sim_page), 12);
    gtk_box_pack_start(GTK_BOX(sim_page), section_label("7. Request resources"), FALSE, FALSE, 0);
    request_process = gtk_combo_box_text_new(); request_vector = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(request_vector), "Vector e.g. 1, 0, 0");
    gtk_box_pack_start(GTK_BOX(sim_page), gtk_label_new("Process"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sim_page), request_process, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sim_page), request_vector, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sim_page), button("Submit resource request", G_CALLBACK(request_resources_clicked)), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sim_page), section_label("Release resources"), FALSE, FALSE, 0);
    release_process = gtk_combo_box_text_new(); release_vector = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(release_vector), "Vector e.g. 0, 1, 0");
    gtk_box_pack_start(GTK_BOX(sim_page), release_process, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sim_page), release_vector, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sim_page), button("Release resources", G_CALLBACK(release_resources_clicked)), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sim_page), section_label("10. Recover from deadlock"), FALSE, FALSE, 0);
    recovery_process = gtk_combo_box_text_new();
    gtk_box_pack_start(GTK_BOX(sim_page), recovery_process, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sim_page), button("Recover selected process", G_CALLBACK(recover_process_clicked)), FALSE, FALSE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll(sim_page), gtk_label_new("Requests & Recovery"));

    rebuild_matrix_editor();
    refresh_all();
    show_output("Welcome. Load the classic example to test Banker’s Algorithm. Expected sequence: P1 -> P3 -> P4 -> P0 -> P2.");
    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new("org.example.deadlocksimulator", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
