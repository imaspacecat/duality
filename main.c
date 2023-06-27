#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include "game.h"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 1000
#define CIRCLE_RADIUS 40
#define RECTANGLE_WIDTH 80
#define DEFAULT_SPEED 3
#define DEFAULT_SPAWN_FREQUENCY 700 // in milliseconds // 700

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
        // TODO make cleaner and more efficient
        if (CheckCollisionCircleRec(*(oB->pose), CIRCLE_RADIUS, current->data->rect))
        {
            printf("blue collision!\n");
            if (!current->data->color) // is blue
                player->score++;
            else
                player->health--;
            Node *temp = current;
            if (current == l->last)
                break;
            current = current->next;
            remove_node(l, temp);
        }
        else if (CheckCollisionCircleRec(*(oR->pose), CIRCLE_RADIUS, current->data->rect))
        {
            if (current->data->color) // is red
                player->score++;
            else
                player->health--;
            Node *temp = current;
            if (current == l->last)
                break;
            current = current->next;
            remove_node(l, temp);
        }

        if (current == l->last)
            break;
        current = current->next;
    }
}

int main(void)
{
    int speed = DEFAULT_SPEED;
    Player player = {10, 0};

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "duality");

    Vector2 blueOrbPose = {(float)SCREEN_WIDTH / 3, (float)SCREEN_HEIGHT * 9 / 10};
    Vector2 redOrbPose = {(float)(2 * SCREEN_WIDTH / 3), (float)SCREEN_HEIGHT * 9 / 10};

    Orb blueOrb = {&blueOrbPose, CIRCLE_RADIUS, 0};
    Orb redOrb = {&redOrbPose, CIRCLE_RADIUS, 1};

    Rectangle rect = {(float)GetRandomValue(RECTANGLE_WIDTH / 2, SCREEN_WIDTH), (float)RECTANGLE_WIDTH / 2,
                      (float)RECTANGLE_WIDTH, (float)RECTANGLE_WIDTH};
    Node *first = create_node(rect, GetRandomValue(0, 1));
    LinkedList rectangles = {first, first};

    // Rendering flags and fps
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    SetTargetFPS(60);

    int frameCount = 0;
    while (!WindowShouldClose())
    {
        // Update
        blueOrbPose.x = GetMouseX();
        redOrbPose.x = GetMouseY() * SCREEN_WIDTH / SCREEN_HEIGHT;
        // printf("x pose : %d\n screenwidth: %d\n", GetMouseX(), GetScreenWidth());
        frameCount++;

        // Make sure player does not go out of area limits
        if ((blueOrbPose.x + CIRCLE_RADIUS) >= GetScreenWidth())
            blueOrbPose.x = GetScreenWidth() - CIRCLE_RADIUS;
        else if (blueOrbPose.x <= CIRCLE_RADIUS)
            blueOrbPose.x = CIRCLE_RADIUS;
        if ((redOrbPose.x + CIRCLE_RADIUS) >= GetScreenWidth())
            redOrbPose.x = GetScreenWidth() - CIRCLE_RADIUS;
        else if (redOrbPose.x <= CIRCLE_RADIUS)
            redOrbPose.x = CIRCLE_RADIUS;

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Blue circle moves with mouse X\nand red circle with mouse Y",
                 20, 20, 20, DARKGRAY);

        char playerInfoS[100]; // TODO put on heap or use strcat instead cause of max length
        sprintf(playerInfoS, "Health: %d\nScore: %d", player.health, player.score);
        DrawText(playerInfoS, 20, 80, 20, RED);

        DrawCircleV(blueOrbPose, CIRCLE_RADIUS, BLUE);
        DrawCircleV(redOrbPose, CIRCLE_RADIUS, RED);

        update(&rectangles, speed, &blueOrb, &redOrb, &player);

        // Spawn enemies
        if ((GetFrameTime() * frameCount) * 1000 >= DEFAULT_SPAWN_FREQUENCY)
        {
            frameCount = 0;
            Rectangle rect = {GetRandomValue(0, SCREEN_WIDTH - RECTANGLE_WIDTH), RECTANGLE_WIDTH / 2,
                              RECTANGLE_WIDTH, RECTANGLE_WIDTH}; // TODO test and fix x limits
            append_node(&rectangles, rect, GetRandomValue(0, 1));
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}