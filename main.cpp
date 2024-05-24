#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Point {
    int x;
    int y;
};

int F1, F2, Z1, Z2;
Point FirstClick;
Point SecondClick;
bool Noted = false;
int phyWidth = 700;
int phyHeight = 700;
int logWidth = 100;
int logHeight = 100;
int centerX = logWidth / 2;
int centerY = logHeight / 2;
int mouseX = centerX, mouseY = centerY;
int mouses = 0;
int alphaX = 3;
int alphaY = 3;
int f = 0, z = 0;
int TimeLeft = 60;
int score = 0;
int highscore=0;
bool newscore=false;
char temp[5];
char TimeLeftr[3];
int matrix[5][5];
bool display1Active = true;

int GetRandomNumber() {
    return rand() % 5 + 1;
}

void timerCallback(int value) {
    if (TimeLeft > 0) {
        TimeLeft--;
    } else {
        display1Active = true;
        TimeLeft = 10;
        score = 0;
    }
    glutPostRedisplay();
    glutTimerFunc(1000, timerCallback, 0);
}
void returnToMenuCallback(int value) {
    display1Active = true;
    GetRandomNumber(); // Reset the game state
    glutPostRedisplay(); // Redisplay the menu
}
void returnToMenuCallback(int value);
void init2D() {
    glClearColor(0.4, 0.7, 1, 1.0); // COLOR BACKGROUND
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, logWidth, 0.0, logHeight);
}

void printSome(const char *str, int x, int y) {
    glColor3f(1, 1.0, 0.0);
    glRasterPos2i(x, y);
    for (int i = 0; i < (int)strlen(str); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
    }
}

void mouseClick(int x, int y) {
    mouseX = x;
    mouseY = phyHeight - y;
    mouseX = 0.5 + 1.0 * mouseX * logWidth / phyWidth;
    mouseY = 0.5 + 1.0 * mouseY * logHeight / phyHeight;
    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y) {
    if (key == GLUT_KEY_UP) z += 1;
    if (key == GLUT_KEY_DOWN) z -= 1;
    if (key == GLUT_KEY_LEFT) f -= 1;
    if (key == GLUT_KEY_RIGHT) f += 1;
    glutPostRedisplay();
}

void DrawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments); // get the current angle
        float x = r * cosf(theta); // calculate the x component
        float y = r * sinf(theta); // calculate the y component
        glVertex2f(x + cx, y + cy); // output vertex
    }
    glEnd();
}

void SwapCircles() {
    int temp = matrix[FirstClick.x][FirstClick.y];
    matrix[FirstClick.x][FirstClick.y] = matrix[SecondClick.x][SecondClick.y];
    matrix[SecondClick.x][SecondClick.y] = temp;
}

bool HasHorizontalMatch(int row, int col) {
    int color = matrix[row][col];
    int count = 1;

    // Check to the left
    for (int i = col - 1; i >= 0; i--) {
        if (matrix[row][i] == color) {
            count++;
        } else {
            break;
        }
    }

    // Check to the right
    for (int i = col + 1; i < 5; i++) {
        if (matrix[row][i] == color) {
            count++;
        } else {
            break;
        }
    }

    return count >= 3;
}

bool HasVerticalMatch(int row, int col) {
    int color = matrix[row][col];
    int count = 1;

    // Check below
    for (int i = row - 1; i >= 0; i--) {
        if (matrix[i][col] == color) {
            count++;
        } else {
            break;
        }
    }

    // Check above
    for (int i = row + 1; i < 5; i++) {
        if (matrix[i][col] == color) {
            count++;
        } else {
            break;
        }
    }

    return count >= 3;
}
void HandleCirclesFalling() {
    // Move circles down
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 4; i++) {
            if (matrix[i][j] == 0) {
                for (int k = i + 1; k < 5; k++) {
                    if (matrix[k][j] != 0) {
                        matrix[i][j] = matrix[k][j];
                        matrix[k][j] = 0;
                        break;
                    }
                }
            }
        }
    }

    // Generate new circles to fill empty spaces at the top
    for (int j = 0; j < 5; j++) {
        for (int i = 4; i >= 0; i--) {
            if (matrix[i][j] == 0) {
                matrix[i][j] = GetRandomNumber();
            }
        }
    }
}

void CheckForMatches() {
    bool foundMatch = false;

    // Check for horizontal matches
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            if (matrix[i][j] != 0 && HasHorizontalMatch(i, j)) {
                foundMatch = true;
                // Update score and remove circles
                score += 10;
                matrix[i][j] = matrix[i][j + 1] = matrix[i][j + 2] = 0;
            }
        }
    }

    // Check for vertical matches
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 3; i++) {
            if (matrix[i][j] != 0 && HasVerticalMatch(i, j)) {
                foundMatch = true;
                // Update score and remove circles
                score += 10;
                TimeLeft+=5;
                matrix[i][j] = matrix[i + 1][j] = matrix[i + 2][j] = 0;
            }
        }
    }

    if (foundMatch) {
        // Call function to make circles fall and generate new circles
        HandleCirclesFalling();
    }
}



void GenerateRandomNumbers() {
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            matrix[x][y] = GetRandomNumber();
        }
    }
}

void NoteFirstPoint() {
    FirstClick.x = f;
    FirstClick.y = z;
    Noted = true;
}

void NoteSecondPoint() {
    if (Noted) {
        SecondClick.x = f;
        SecondClick.y = z;

        if (abs(FirstClick.x - SecondClick.x) <= 1 && abs(FirstClick.y - SecondClick.y) <= 1) {
            SwapCircles();
        }
        Noted = false;
        CheckForMatches();
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseClick(x, y);
        if (display1Active && mouseX > 42 && mouseX < 59 && mouseY > 50 && mouseY < 55) {
            display1Active = false;
            GenerateRandomNumbers();
            glutTimerFunc(1000, timerCallback, 0);
        } else if (!display1Active) {
            if (!Noted) {
                NoteFirstPoint();
            } else {
                NoteSecondPoint();
            }
            if (score>highscore)
            {
                highscore=score;
                newscore=true;
        }
    }
}

}

static void DisplayMenu(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0, 0.7, 0.5);
    glColor3f(0.4, 1, 0.4);

    int o, l;
    for (o = 0; o < 100; o += 10) {
        for (l = 0; l < 100; l += 15) {
            glBegin(GL_LINE_STRIP);
            glVertex2i(o, l);
            glVertex2i(o + 10, l + 15);
            glEnd();
        }
    }

    printSome("Circle Crush", 42, 60);
    glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_QUADS);
    glVertex2i(42, 55);
    glVertex2i(42, 50);
    glVertex2i(59, 50);
    glVertex2i(59, 55);
    glEnd();
    if(newscore)
    {
        glColor3f(1,1,0);
        printSome("New Highscore!",40,40);
        sprintf(temp,"%d",highscore);
        printSome(temp,49, 35);
        glEnd();
        newscore=false;
    }
    glColor3f(1, 1, 0);
    printSome("Play!", 47, 51.5);
    glutSwapBuffers();
}

static void DisplayGame(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0, 0.7, 0.5);
    glColor3f(0.4, 1, 0.4);

    int o, l;
    for (o = 0; o < 100; o += 10) {
        for (l = 0; l < 100; l += 15) {
            glBegin(GL_LINE_STRIP);
            glVertex2i(o, l);
            glVertex2i(o + 10, l + 15);
            glEnd();
        }
    }
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2i(20, 10);
    glVertex2i(95, 10);
    glVertex2i(95, 90);
    glVertex2i(20, 90);
    glEnd();

    for (int i = 35; i < 95; i += 15) {
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
        glVertex2i(i, 90);
        glVertex2i(i, 10);
        glEnd();
    }

    for (int i = 10; i < 90; i += 16) {
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
        glVertex2i(20, i);
        glVertex2i(95, i);
        glEnd();
    }

    if (mouseX > 20 && mouseX < 95 && mouseY > 10 && mouseY < 90) {
        if (mouseX >= 20 && mouseX < 35) f = 0;
        else if (mouseX >= 35 && mouseX < 50) f = 1;
        else if (mouseX >= 50 && mouseX < 65) f = 2;
        else if (mouseX >= 65 && mouseX < 80) f = 3;
        else if (mouseX >= 80 && mouseX <= 95) f = 4;

        if (mouseY >= 10 && mouseY < 26) z = 0;
        else if (mouseY >= 26 && mouseY < 42) z = 1;
        else if (mouseY >= 42 && mouseY < 58) z = 2;
        else if (mouseY >= 58 && mouseY < 74) z = 3;
        else if (mouseY >= 74 && mouseY < 90) z = 4;
    }

    glColor3f(1, 0.84, 0);
    glBegin(GL_QUADS);
    glVertex2i(20 + (f * 15), 10 + (z * 16)); // Bottom-left
    glVertex2i(20 + ((f + 1) * 15), 10 + (z * 16)); // Bottom-right
    glVertex2i(20 + ((f + 1) * 15), 10 + ((z + 1) * 16)); // Top-right
    glVertex2i(20 + (f * 15), 10 + ((z + 1) * 16)); // Top-left
    glEnd();

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            switch (matrix[i][j]) {
                case 1:
                    glColor3f(1, 0, 0);
                    break;
                case 2:
                    glColor3f(0, 1, 0);
                    break;
                case 3:
                    glColor3f(0, 0, 1);
                    break;
                case 4:
                    glColor3f(1, 0, 1);
                    break;
                case 5:
                    glColor3f(0, 1, 1);
                    break;
            }
            DrawCircle((i + 27.5) + (i * 15) - i, (j + 18) + (j * 16) - j, 6, 20);
        }
    }
    glColor3f(0.31, 0.31, 0.31);
    glBegin(GL_QUADS);
    glVertex2i(1, 60);
    glVertex2i(15, 60);
    glVertex2i(15, 90);
    glVertex2i(1, 90);
    glEnd();
    glColor3f(1, 1, 1);
    printSome("Time left", 1.5, 70);
    glColor3f(1, 1, 1);
    printSome("score", 1.5, 85);
    sprintf(temp, "%d", score);
    printSome(temp, 2, 80);
    sprintf(TimeLeftr, "%d", TimeLeft);
    printSome(TimeLeftr, 2, 65);
    if (score >= 80) {
        glColor3f(1, 0, 0);
        printSome("Congratulations! You reached 80 points!", 20, 95);
        glutSwapBuffers();
        glutTimerFunc(5000, returnToMenuCallback, 0); // Return to menu after 5 seconds
        return;
    }
    glFlush();
    glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int xx, int yy) {
    if (key == 27) exit(0);
}

static void display(void) {
    if (display1Active) {
        DisplayMenu();
    } else {
        DisplayGame();
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(phyWidth, phyHeight);
    glutCreateWindow("Circle Crush");
    init2D();
    glutDisplayFunc(display);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(specialKeyboard);
    glutMouseFunc(mouse);
    glutMainLoop();
    void returnToMenuCallback(int value);
    return 0;
}
