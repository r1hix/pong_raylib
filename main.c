#include <raylib.h>
#include <raymath.h>

// === Constants & Definitions ===
#define windowWidth 1200
#define windowHeight 800

typedef enum Scene {
    StartScreen, GameScene, EndScreen
} Scene;

// === Structs ===

typedef struct PaddleControls {
    int key_Up, key_Down;
} PaddleControls;

typedef struct Paddle {
    int playerID;
    float width, height, xPos, yPos;
    PaddleControls controls;
    Rectangle body;
    Color color;
} Paddle;

typedef struct Ball {
    float radius;
    Vector2 position, velocity;
    Color color;
} Ball;


// === Constants ===
Color ballColor = BLUE;
Color paddleColor = BLACK;

const float paddleSpeed = 6.0f;
const float paddleInitialWidth = 15.0f;
const float paddleIntitalHeight = 120.0f;

const float paddle1InitialPosX = 30.0f;
const float paddle1InitialPosY = windowHeight / 2.0f - paddleIntitalHeight/2.0f;
const float paddle2InitialPosX = windowWidth - paddle1InitialPosX - paddleInitialWidth;
const float paddle2InitialPosY = windowHeight / 2.0f - paddleIntitalHeight/2.0f;

const float ballSpeed = 10.0f;
const float ballInitialRadius = 10.0f;

// === Functions ===

void UpdatePaddle(Paddle *paddle) {
    // - Input -
    if (IsKeyDown(paddle->controls.key_Up)) {
        paddle->yPos -= paddleSpeed;
    }
    if (IsKeyDown(paddle->controls.key_Down)) {
        paddle->yPos += paddleSpeed;
    }

    // - Wall Constraints - 
    if (paddle->yPos < 0)
        paddle->yPos = 0;
    if (paddle->yPos + paddle->height > windowHeight)
        paddle->yPos = windowHeight - paddle->height;

    
}

void UpdateBall(Ball *ball, Paddle *p1, Paddle *p2) {
    // Basic Movement
    ball->position.x += ball->velocity.x;
    ball->position.y += ball->velocity.y;

    // Wall Collison
    if (ball->position.y - ball->radius/2 < 0) {
        ball->position.y = 0 + ball->radius/2;
        ball->velocity.y = -ball->velocity.y;
    }
    if (ball->position.y + ball->radius/2 > windowHeight) {
        ball->position.y = windowHeight - ball->radius/2;
        ball->velocity.y = -ball->velocity.y;
    }

    // Paddle Collision
    if (CheckCollisionCircleRec(ball->position, ball->radius, p1->body)) {
        DrawText("COLLISION BW P1 AND BALL", 0, 0, 30, RED);
    }
    if (CheckCollisionCircleRec(ball->position, ball->radius, p2->body)) {
        DrawText("COLLISION BW P2 AND BALL", 0, 0, 30, RED);
    }
}


int main(void)  {
    InitWindow(windowWidth, windowHeight, "Pong");
    SetTargetFPS(60);
    
    Paddle paddle1 = {
        .playerID = 1,
        .xPos = paddle1InitialPosX,
        .yPos = paddle1InitialPosY,
        .color = paddleColor,
        .width = paddleInitialWidth,
        .height = paddleIntitalHeight,
        .body = {
            .height = paddle1.height,
            .width = paddle1.width,
            .x = paddle1.xPos,
            .y = paddle1.yPos
        },
        .controls = {
            .key_Up = KEY_W,
            .key_Down = KEY_S
        }
    };

    Paddle paddle2 = {
        .playerID = 2,
        .xPos = paddle2InitialPosX,
        .yPos = paddle2InitialPosY,
        .color = paddleColor,
        .width = paddleInitialWidth,
        .height = paddleIntitalHeight,
        .body = {
            .height = paddle2.height,
            .width = paddle2.width,
            .x = paddle2.xPos,
            .y = paddle2.yPos
        },
        .controls = {
            .key_Up = KEY_UP,
            .key_Down = KEY_DOWN
        }
    };

    Ball ball = {
        .position = {windowWidth/2 , windowHeight/2},
        .radius = ballInitialRadius,
        .velocity = {-ballSpeed, 0},
        .color = BLUE
    };

    while (!WindowShouldClose())
    {
        UpdatePaddle(&paddle1);
        UpdatePaddle(&paddle2);
        UpdateBall(&ball, &paddle1, &paddle2);

        // === Drawing ===
        BeginDrawing();
        ClearBackground(WHITE);
        DrawCircle(ball.position.x, ball.position.y, ball.radius, ball.color);
        DrawRectangle(paddle1.xPos, paddle1.yPos, paddle1.width, paddle1.height, paddle1.color);
        DrawRectangle(paddle2.xPos, paddle2.yPos, paddle2.width, paddle2.height, paddle2.color);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}