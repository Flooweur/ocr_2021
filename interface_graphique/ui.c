#include <gtk/gtk.h>

// State of the game.
typedef enum State
{
    UNSOLVED,                       // Unsolved sudoku state
    SOLVE,                          // Solved sudoku state
} State;

// Structure of the disc.
typedef struct Disc
{
    GdkRectangle rect;              // Position and size
    GdkPoint step;                  // Horizontal and verical steps in pixels
    guint period;                   // Period in milliseconds
    guint event;                    // Event ID used to move the disc
} Disc;

// Structure of the graphical user interface.
typedef struct UserInterface
{
    GtkWindow* window;              // Main window
    GtkDrawingArea* area;           // Drawing area
    GtkButton* solve_button;        // Solve button
    GtkWidget* entry;               // User enters the unsolved sudoku picture filename
} UserInterface;

// Structure of the game.
typedef struct Game
{
    State state;                    // State of the game
    Disc disc;                      // Disc
    UserInterface ui;               // User interface
} Game;



char* filename_unsolved = "../solveur/grid_00";
char* filename_solved = "../solveur/grid_00.result";

void read_sudoku(char* filename,char* l) 
{
	FILE *f = fopen(filename,"r");
	if(f == NULL)
		return;
	char ch = fgetc(f);
    int i = 0;
    while (ch != EOF)
    {
        if (ch != ' ' && ch != '\n') {
            printf("valeur de ch %i = %c\n", i, ch);
            l[i] = ch;
            i++;
        }
        ch = fgetc(f);
    }
    fclose(f);
    printf("string: %s\n", l);
    printf("size : %i\n",i);
}
/*
void read_sudoku2(FILE* f,char* l) 
{
    if (f == NULL) {
        printf("error");
    }
    char ch = fgetc(f);
    int i = 0;
    while (ch != EOF)
    {
        if (ch != ' ' && ch != '\n') {
            printf("valeur de ch %i = %c\n", i, ch);
            l[i] = ch;
            i++;
        }
        ch = fgetc(f);
    }
    fclose(f);
    printf("string: %s\n", l);
    printf("size : %i\n",i);
}
*/
// Event handler for the "draw" signal of the drawing area.
gboolean on_draw(GtkWidget* widget, cairo_t* cr, gpointer user_data)
{
    // Gets the 'Game' structure.
    Game* game = user_data;

    // Sets the background to black.
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    // Draws the edges in black.
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, game->disc.rect.x, game->disc.rect.y,
        game->disc.rect.width, game->disc.rect.height);
    cairo_fill(cr);

    // Vertical lines.
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, game->disc.rect.x, game->disc.rect.y,
        7, game->disc.rect.height);
    cairo_fill(cr);
    for (size_t i = 1; i < 10; i++) {
        cairo_set_source_rgb(cr, 1, 1, 1);
        if (i % 3 == 0) {
            cairo_rectangle(cr, (game->disc.rect.width / 9) * i + (game->disc.rect.x), game->disc.rect.y,
                7, game->disc.rect.height);
        }
        else {
            cairo_rectangle(cr, (game->disc.rect.width / 9) * i + (game->disc.rect.x), game->disc.rect.y,
                3, game->disc.rect.height);
        }
        cairo_fill(cr);
    }

    // Horizontal lines.
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, game->disc.rect.x, game->disc.rect.y,
        game->disc.rect.height, 7);
    cairo_fill(cr);
    for (size_t i = 1; i < 10; i++) {
        cairo_set_source_rgb(cr, 1, 1, 1);
        if (i % 3 == 0) {
            cairo_rectangle(cr, game->disc.rect.x, (game->disc.rect.height / 9) * i + (game->disc.rect.y),
                game->disc.rect.height, 7);
        }
        else {
            cairo_rectangle(cr, game->disc.rect.x, (game->disc.rect.height / 9) * i + (game->disc.rect.y),
                game->disc.rect.height, 3);
        }
        cairo_fill(cr);
    }
    cairo_set_font_size(cr, 30);

     char l[81] = ".................................................................................";

     char l2[81] = ".................................................................................";
    
    printf("string before : %s\n",l);
    read_sudoku(filename_unsolved,l);
    read_sudoku(filename_solved,l2);
    printf("string after l: %s\n",l);

    int x = 50;
    int y = 70;

    char i =l[0];
    int j = 0;
    while(j <81)
    {
	    i = l[j]; 
	    if (i > '0' && i <= '9')
	    {
		    cairo_set_source_rgb(cr, 1, 1, 1);
		    cairo_move_to(cr, x, y);
		    char num[] = {i,'\0'};
		    cairo_show_text(cr,num);
		    printf("valeur de i pour j = %i: %s, (x: %i, y:%i) \n",j,num,x,y);
	    }
	    else if( i == '.')
	    {
		    cairo_set_source_rgb(cr,1,0,0);
		    cairo_move_to(cr,x,y);
		    char num[] = {l2[j],'\0'};
		    cairo_show_text(cr,num);
	    }
	    j++;
	    if (j % 9 == 0)
	    {
		    x = 50;
		    y +=55;
	    }
	    else 
	    {	
		    x += 55;
	    }
    }
    return TRUE;
}

//gboolean draw_digits(GtkWidget* widget, cairo_t* cr, gpointer user_data) {


    // Save image to .jpg file
    //GtkAllocation size;
    //Pixbuf pixbuf;
    //string[] jpegOptions; string[] jpegOptionValues;
    //getAllocation(size); // grab the widget's size as allocated by its parent
    //pixbuf = getFromSurface(context.getTarget(), xOffset, yOffset, size.width, size.height); // the contents of the surface go into the buffer

    // prep and write JPEG file
    //jpegOptions = ["quality"];
    //jpegOptionValues = ["100"];

    //if (pixbuf.savev("./solved_sudoku.jpg", "jpeg", jpegOptions, jpegOptionValues))
    //{
        //gprint("JPEG was successfully saved.");

    //}

//}
//
//
/*
gboolean set_solve(GtkButton* button, gpointer user_data)
{
    Game* game = user_data;

    cairo_t* cr = cairo_create(GTK_DRAWING_AREA(game->ui.area));
    if (game->state == UNSOLVED) 
    {
        game->state = SOLVE;
        gtk_button_set_label(GTK_BUTTON(game->ui.solve_button), "Unsolved");
    }
    else 
    {
        game->state = UNSOLVED;
        gtk_button_set_label(GTK_BUTTON(game->ui.solve_button), "Solve");
    }
    if(cr == NULL)
	    printf("Null cairo");
    // Writing digit
      
}
*/

void set_unsolved(Game* game)
{
    game->state = UNSOLVED;
    gtk_button_set_label(game->ui.solve_button, "Unsolved");
}

// Event handler for the "clicked" signal of the start button.
/*void on_start(GtkButton* button, gpointer user_data)
{
    // Gets the 'Game' structure
    Game* game = user_data;

    switch (game->state)
    {
    case UNSOLVED: set_solve(game); break;
    case SOLVE: set_unsolved(game); break;
    };
}*/

int main(int argc, char* argv[])
{
    // Initializes GTK.
    gtk_init(NULL, NULL);

    // Constructs a GtkBuilder instance.
    GtkBuilder* builder = gtk_builder_new();

    // Loads the UI description.
    // (Exits if an error occurs.)
    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "sudoku.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Gets the widgets.
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "org.gtk.sudoku"));
    GtkDrawingArea* area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));
    GtkButton* solve_button = GTK_BUTTON(gtk_builder_get_object(builder, "solve_button"));


    //GtkWidget* entry = gtk_entry_new();
    //gtk_entry_set_text(GTK_ENTRY(entry), "Type here...");
    //const gchar* filename = gtk_entry_get_text(GTK_ENTRY(entry));


    // Creates the "Game" structure.
    Game game =
    {
        .state = UNSOLVED,

        .disc =
            {
                .rect = { 30, 30, 500, 500 },
                .step = { 1, 1 },
                .event = 0,
            },

        .ui =
            {
                .window = window,
                .area = area,
                .solve_button = solve_button,
            },
    };

    // Connects event handlers.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(area, "draw", G_CALLBACK(on_draw), &game);
    //g_signal_connect(solve_button, "clicked", G_CALLBACK(set_solve), &game);
    //g_signal_connect(entry, "activate", G_CALLBACK(on_start), entry);

    //gtk_container_add(GTK_CONTAINER(window), entry);

    // Runs the main loop.
    gtk_main();

    // Exits.
    return 0;
}
