#include "book_storage.c"
#include <strings.h>
#include <gtk/gtk.h>
// compilation: 
// cc `pkg-config --cflags gtk+-3.0` projekt.c -o projekt.exe `pkg-config --libs gtk+-3.0`
// in msys2 mingw64 shell

//Initialize application entries
GtkEntryBuffer *author_buff, *title_buff, *count_buff, *price_buff, *file_buff;
GtkWidget *book_author, *book_title, *book_count, *book_price, *file_path;

// Initialize storage
book* storage = NULL;
book* current_book = NULL;


static void print_hello(GtkWidget *widget, gpointer data)
{

  g_print("Hello World\n");
}

void add_book(GtkWidget *widget, gpointer data)
{
  char *author = (char*) gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(author_buff));
  char *title = (char*) gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(title_buff));
  char *price = (char*) gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(price_buff));
  char *count = (char*) gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(count_buff));

  double priceD = strtod(price, NULL);
  int countI = atoi(count);
  bool result = pushBackBook(&storage, title, author, priceD, countI);
  
  if (DEBUG == 1)
  {
    showAllBooks(&storage);
    if (result == 1){
      g_print("ADDED BOOK: %s;%s;%.2f;%d\n", title, author, priceD, countI);
    }
    else
    {
      g_print("FAILED TO ADD BOOK\n");
    }
  }

  if (result == 1)
  {
    findBook(&storage, title, author, &current_book);
    char strPrice[50];
    char strCount[50];
    sprintf(strPrice, "%.2f", current_book->price);
    sprintf(strCount, "%d", current_book->amount);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(price_buff), strPrice, 50);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(count_buff), strCount, 50);
    GtkWidget *alert;
    alert = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "BOOK ADDED");
    gtk_dialog_run(GTK_DIALOG(alert));
    gtk_widget_destroy(alert);
  }
  else
  {
    GtkWidget *alert;
    alert = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "COULDN'T ADD BOOK");
    gtk_dialog_run(GTK_DIALOG(alert));
    gtk_widget_destroy(alert);
  }


  g_print("\n");
}

static void show_book(GtkWidget *widget, gpointer data)
{
  char *author =  (char*) gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(author_buff));
  char *title = (char*) gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(title_buff));
  book* foundBook = NULL;

  bool result = findBook(&storage, title, author, &foundBook);
  if (DEBUG == 1)
  {
    if (result == 1){
      g_print("FOUND BOOK: %s;%s;%.2f;%d\n", foundBook->title, foundBook->author, foundBook->price, foundBook->amount);
    }
    else
    {
      g_print("BOOK NOT FOUND\n");
    }
  }

  if (result == 1)
  {
    char strPrice[50];
    char strCount[50];
    sprintf(strPrice, "%.2f", foundBook->price);
    sprintf(strCount, "%d", foundBook->amount);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(price_buff), strPrice, 50);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(count_buff), strCount, 50);
    current_book = foundBook;
  }
  else
  {
    GtkWidget *alert;
    alert = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "BOOK NOT FOUND");
    gtk_dialog_run(GTK_DIALOG(alert));
    gtk_widget_destroy(alert);
  }
}

static void delete_book(GtkWidget *widget, gpointer data)
{
  char *author = (char*) gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(author_buff));
  char *title = (char*) gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(title_buff));

  bool result = removeBook(&storage, title, author);
  if (result == 1)
  {
    if (strcasecmp(current_book->title, title) && strcasecmp(current_book->author, author))
    {
      gtk_entry_set_text(GTK_ENTRY(book_author), "AUTHOR");
      gtk_entry_set_text(GTK_ENTRY(book_title), "TITLE");
      gtk_entry_set_text(GTK_ENTRY(book_price), "PRICE");
      gtk_entry_set_text(GTK_ENTRY(book_count), "AMOUNT");
    }
    GtkWidget *alert;
    alert = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "BOOK DELETED");
    gtk_dialog_run(GTK_DIALOG(alert));
    gtk_widget_destroy(alert);
  }
  else
  {
    GtkWidget *alert;
    alert = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "BOOK NOT FOUND");
    gtk_dialog_run(GTK_DIALOG(alert));
    gtk_widget_destroy(alert);
  }
  current_book = storage;
}

static void show_next(GtkWidget *widget, gpointer data)
{
  if (current_book == NULL || current_book->next == NULL) 
  {
    return;
  }
  current_book = current_book->next;
  char strPrice[50];
  char strCount[50];
  sprintf(strPrice, "%.2f", current_book->price);
  sprintf(strCount, "%d", current_book->amount);
  gtk_entry_set_text(GTK_ENTRY(book_author), current_book->author);
  gtk_entry_set_text(GTK_ENTRY(book_title), current_book->title);
  gtk_entry_set_text(GTK_ENTRY(book_price), strPrice);
  gtk_entry_set_text(GTK_ENTRY(book_count), strCount);
}

static void show_previous(GtkWidget *widget, gpointer data)
{
  if (current_book == NULL) 
  {
    return;
  }
  book *iterator = storage;
  while (iterator->next != current_book)
  {
    if (DEBUG == 1)
    {
      g_print("STORAGE-ITERATOR: %p-%p\n", storage, iterator);
    }

    if (iterator->next == NULL){
      return;
    }
    iterator = iterator->next;
  }
  current_book = iterator;
  char strPrice[50];
  char strCount[50];
  sprintf(strPrice, "%.2f", current_book->price);
  sprintf(strCount, "%d", current_book->amount);
  gtk_entry_set_text(GTK_ENTRY(book_author), current_book->author);
  gtk_entry_set_text(GTK_ENTRY(book_title), current_book->title);
  gtk_entry_set_text(GTK_ENTRY(book_price), strPrice);
  gtk_entry_set_text(GTK_ENTRY(book_count), strCount);
}

static void list_books(GtkWidget *widget, gpointer data)
{
    if (storage == NULL)
    {
        return;
    }
    book *iterator = storage;
    char books[2048];


    int i = 0;
    while (iterator != NULL)
    {
      char * book;
      if (0 > __mingw_asprintf(&book, "Book no. %d: %s %s %d %.2f\n",
           ++i, iterator->author, iterator->title,
           iterator->amount, iterator->price))
      {
        return;
      }
      strcat(books, book);
      iterator = iterator->next;
    }

    GtkWidget *alert;
    alert = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, books);
    gtk_dialog_run(GTK_DIALOG(alert));
    gtk_widget_destroy(alert);

}

static void _load_from_csv_file(GtkWidget *widget, gpointer data)
{
  GtkWidget *alert;
  g_print("loading file: %s\n", gtk_entry_buffer_get_text(file_buff));
  if (loadFromCsv(gtk_entry_buffer_get_text(file_buff), &storage))
  {
    alert = gtk_message_dialog_new(GTK_WINDOW(widget), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "SUCCESSFULLY LOADED FROM CSV");
  }
  else
  {
    alert = gtk_message_dialog_new(GTK_WINDOW(widget), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "ERROR WHEN LOADING FROM CSV");
  }
  gtk_dialog_run(GTK_DIALOG(alert));
  gtk_widget_destroy(alert);
  gtk_widget_destroy(widget);
  return;
}

static void load_from_csv(GtkWidget *widget, gpointer data)
{
  GtkWidget *dialog, *content_area;
  dialog = gtk_dialog_new_with_buttons("Get Text",
                                       NULL,
                                       GTK_DIALOG_MODAL,
                                       GTK_STOCK_OK,
                                       GTK_RESPONSE_OK,
                                       NULL);
  content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  file_buff = gtk_entry_buffer_new("", 0);
  file_path = gtk_entry_new_with_buffer(GTK_ENTRY_BUFFER(file_buff));
  gtk_entry_set_text(GTK_ENTRY(file_path), "path/to/csv/file.txt");
  gtk_container_add(GTK_CONTAINER(content_area), file_path);

  gtk_widget_show_all(dialog);
  g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(_load_from_csv_file), dialog);
}

static void activate(GtkApplication *app, gpointer data)
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *button_box;
  GtkWidget *main_box;
  GtkWidget *menu_bar;
  GtkWidget *main_menu;
  GtkWidget *book_info;


  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Library?");
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
  gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

  main_box = gtk_hbox_new(FALSE, 10);
  gtk_container_add(GTK_CONTAINER(window), main_box);


  // menu_add_book = gtk_menu_item_new_with_label("Add book");
  // menu_remove_book = gtk_menu_item_new_with_label("Remove book");
  // menu_show_book = gtk_menu_item_new_with_label("Show book");

  // main_menu = gtk_menu_new();

  // gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_add_book);
  // gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_remove_book);
  // gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_show_book);

  button_box = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
  gtk_container_add(GTK_CONTAINER(main_box), button_box);

  button = gtk_button_new_with_label("Add book");
  g_signal_connect(button, "clicked", G_CALLBACK(add_book), NULL);
  gtk_container_add(GTK_CONTAINER(button_box), button);
  
  button = gtk_button_new_with_label("Find book");
  g_signal_connect(button, "clicked", G_CALLBACK(show_book), NULL);
  gtk_container_add(GTK_CONTAINER(button_box), button);

  button = gtk_button_new_with_label("Delete book");
  g_signal_connect(button, "clicked", G_CALLBACK(delete_book), NULL);
  gtk_container_add(GTK_CONTAINER(button_box), button);

  button = gtk_button_new_with_label("Show next book");
  g_signal_connect(button, "clicked", G_CALLBACK(show_next), NULL);
  gtk_container_add(GTK_CONTAINER(button_box), button);

  button = gtk_button_new_with_label("Show previous book");
  g_signal_connect(button, "clicked", G_CALLBACK(show_previous), NULL);
  gtk_container_add(GTK_CONTAINER(button_box), button);

  button = gtk_button_new_with_label("List all books");
  g_signal_connect(button, "clicked", G_CALLBACK(list_books), NULL);
  gtk_container_add(GTK_CONTAINER(button_box), button);

  button = gtk_button_new_with_label("Load from csv");
  g_signal_connect(button, "clicked", G_CALLBACK(load_from_csv), NULL);
  gtk_container_add(GTK_CONTAINER(button_box), button);

  button = gtk_button_new_with_label("Exit");
  g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), window);
  gtk_container_add(GTK_CONTAINER(button_box), button);

  book_info = gtk_vbox_new(FALSE, 10);
  gtk_container_add(GTK_CONTAINER(main_box), book_info);

  author_buff = gtk_entry_buffer_new("", 0);
  book_author = gtk_entry_new_with_buffer(GTK_ENTRY_BUFFER(author_buff));
  gtk_entry_set_text(GTK_ENTRY(book_author), "AUTHOR");
  gtk_container_add(GTK_CONTAINER(book_info), book_author);
  
  title_buff = gtk_entry_buffer_new("", 0);
  book_title = gtk_entry_new_with_buffer(GTK_ENTRY_BUFFER(title_buff));
  gtk_entry_set_text(GTK_ENTRY(book_title), "TITLE");
  gtk_container_add(GTK_CONTAINER(book_info), book_title);
  
  price_buff = gtk_entry_buffer_new("", 0);
  book_price = gtk_entry_new_with_buffer(GTK_ENTRY_BUFFER(price_buff));
  gtk_entry_set_text(GTK_ENTRY(book_price), "PRICE");
  gtk_container_add(GTK_CONTAINER(book_info), book_price);
  
  count_buff = gtk_entry_buffer_new("", 0);
  book_count = gtk_entry_new_with_buffer(GTK_ENTRY_BUFFER(count_buff));
  gtk_entry_set_text(GTK_ENTRY(book_count), "AMOUNT");
  gtk_container_add(GTK_CONTAINER(book_info), book_count);
  

  gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{

  GtkApplication *app;
  int status;

  app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}