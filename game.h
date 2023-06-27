#include "raylib.h"

typedef struct
{
    int health;
    int score;
} Player;

typedef struct
{
    Rectangle rect;
    int color;
} Tile;

typedef struct
{
    Vector2 *pose;
    int radius;
    int color;
} Orb;

typedef struct Node
{
    Tile *data;
    struct Node *next;
} Node;

typedef struct
{
    Node *first;
    Node *last;
} LinkedList;