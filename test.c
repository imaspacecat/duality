#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include "game.h"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 1000
#define CIRCLE_RADIUS 40
#define RECTANGLE_WIDTH 80
#define DEFAULT_SPEED 3
#define DEFAULT_SPAWN_FREQUENCY 700 // in milliseconds

Node *create_node(Rectangle rect, int color)
{
    Tile *tile = malloc(sizeof(Tile));
    tile->rect = rect;
    tile->color = color;

    Node *node = malloc(sizeof(Node));
    node->data = tile;

    return node;
}

void free_node(Node *node)
{
    free(node->data);
    free(node);
}

void append_node(LinkedList *l, Rectangle rect, int color)
{
    Node *node = create_node(rect, color);

    l->last->next = node;
    l->last = node;
}

void append_node_explicit(LinkedList *l, Node *node) 
{
    l->last->next = node;
    l->last = node;
}

void remove_node(LinkedList *l, Node *node)
{
    Node *current = l->first;

    while (1)
    {
        if (current == node)
        {
            if (current->next == current)
                l->first = NULL;
            else
                l->first = current->next;
            free_node(current);
            break;
        }
        else if (current->next == node)
        {
            current->next = current->next->next;
            free_node(node);
        }

        if (current == l->last)
            break;
        current = current->next;
    }
}

void print_list(LinkedList *l)
{
    int count = 0;
    Node *current = l->first;

    while (1)
    {

        printf("n: %d, Tile: {Rectangle: {x: %f, y: %f, width: %f, height: %f}}\n color: %d\n",
               ++count, current->data->rect.x, current->data->rect.y, current->data->rect.width,
               current->data->rect.height, current->data->color);
        if (current == l->last)
            break;
        current = current->next;
    }
}

Color int_to_color(int color)
{
    if (color)
        return RED;
    else
        return BLUE;
}

void update(LinkedList *l, int speed, Orb *oB, Orb *oR, Player *player)
{
    Node *current = l->first;

    while (1)
    {

        // Draw rects
        current->data->rect.y += speed;
        DrawRectangleRec(current->data->rect, int_to_color(current->data->color));

        // Check for collisions
        // TODO make cleaner
        if (CheckCollisionCircleRec(*(oB->pose), CIRCLE_RADIUS, current->data->rect))
        {
            printf("blue collision!\n");
            if (!current->data->color) // is blue
                player->score++;
            else
                player->health--;
            remove_node(l, current);
        } else if (CheckCollisionCircleRec(*(oR->pose), CIRCLE_RADIUS, current->data->rect))
        {
            if (current->data->color) // is red
                player->score++;
            else
                player->health--;
            remove_node(l, current);
        }

        if (current == l->last)
            break;
        current = current->next;
    }
}

int main(void)
{
    Rectangle rect = {(float)GetRandomValue(RECTANGLE_WIDTH / 2, SCREEN_WIDTH), (float)RECTANGLE_WIDTH / 2,
                      (float)RECTANGLE_WIDTH, (float)RECTANGLE_WIDTH};
    Node *node1 = create_node(rect, GetRandomValue(0, 1));
    LinkedList rectangles = {node1, node1};

    Node *node2 = create_node(rect, GetRandomValue(2, 4));
    append_node_explicit(&rectangles, node2);

    Node *node3 = create_node(rect, GetRandomValue(0, 1));
    append_node_explicit(&rectangles, node3);
    print_list(&rectangles);
    remove_node(&rectangles, node2);
    print_list(&rectangles);

    

    return 0;
}