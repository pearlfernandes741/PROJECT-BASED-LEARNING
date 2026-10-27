#include <gtk/gtk.h>
#include <cairo.h>
#include "algorithms.h"
#include "graph.h"

static gboolean draw_graph(
    GtkWidget *widget,
    cairo_t *cr,
    gpointer data)
{
    (void)data;

    GtkAllocation area;
    gtk_widget_get_allocation(widget, &area);

    double width = area.width;
    double height = area.height;

    if (width < 100 || height < 100)
        return FALSE;

    double process_x = width * 0.20;
    double resource_x = width * 0.80;

    double process_gap =
        height / (process_count + 1.0);

    double resource_gap =
        height / (resource_count + 1.0);

    /* Draw resource allocation edges. */
    for (int p = 0; p < process_count; p++) {
        double py = process_gap * (p + 1);

        for (int r = 0; r < resource_count; r++) {
            if (allocation[p][r] <= 0)
                continue;

            double ry = resource_gap * (r + 1);

            cairo_set_source_rgb(cr, 0.05, 0.55, 0.48);
            cairo_set_line_width(cr, 1.5);
            cairo_move_to(cr, resource_x - 22, ry);
            cairo_line_to(cr, process_x + 28, py);
            cairo_stroke(cr);
        }
    }

    /* Draw process nodes. */
    for (int p = 0; p < process_count; p++) {
        double y = process_gap * (p + 1);
        char label[16];

        g_snprintf(label, sizeof(label), "P%d", p);

        cairo_set_source_rgb(cr, 0.31, 0.27, 0.90);
        cairo_rectangle(
            cr, process_x - 28, y - 14, 56, 28);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_select_font_face(
            cr, "Sans",
            CAIRO_FONT_SLANT_NORMAL,
            CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 12);

        cairo_text_extents_t ext;
        cairo_text_extents(cr, label, &ext);

        cairo_move_to(
            cr, process_x - ext.width / 2, y + 4);
        cairo_show_text(cr, label);
    }

    /* Draw resource nodes. */
    for (int r = 0; r < resource_count; r++) {
        double y = resource_gap * (r + 1);
        char label[16];

        g_snprintf(label, sizeof(label), "R%d", r);

        cairo_set_source_rgb(cr, 0.05, 0.55, 0.48);
        cairo_rectangle(
            cr, resource_x - 22, y - 18, 44, 36);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_set_font_size(cr, 12);

        cairo_text_extents_t ext;
        cairo_text_extents(cr, label, &ext);

        cairo_move_to(
            cr, resource_x - ext.width / 2, y + 4);
        cairo_show_text(cr, label);
    }

    return FALSE;
}

GtkWidget *create_resource_allocation_graph(void)
{
    GtkWidget *area = gtk_drawing_area_new();

    gtk_widget_set_size_request(area, 450, 300);

    g_signal_connect(
        area,
        "draw",
        G_CALLBACK(draw_graph),
        NULL);

    return area;
}
