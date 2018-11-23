#ifdef __APPLE__
#include <GLUT/glut.h>
#else

#include <GL/glut.h>

#endif

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
//# include <conio.h>
# include <stdarg.h>
# include <string.h>
# include <time.h>
# include <limits.h>
# include "isosurface.h"
# include <errno.h>
#include <ctype.h>

int flag = 0;


#define LEFT_MOUSE_BUTTON_BIT   1
#define MIDDLE_MOUSE_BUTTON_BIT 2
#define RIGHT_MOUSE_BUTTON_BIT  4

int mbs;
int mouse_prev_x, mouse_prev_y;
int moving = 0;

static float angle = 0.0;
GLfloat material[] = {0.0, 0, 0.5};

char key;
//* Transformation states
float rot[3], trans[3];
float rot_inc[3], trans_inc[3];
float rot_matrix[3];
double rot_matrix_int[3];
double rot_matrix_inc[3];

#define MAX_ENTER 1024
#define MAX_ISOVALUE 9

double *isovalueArray;

int isovalue_len;
int selected_index = -1;

void processSpecialKeys(int key, int x, int y);

GLfloat materials[9][4] = {
        {1,   0,   0,   1},
        {0,   1,   0,   1},
        {0,   0,   1,   1},
        {0.5, 0,   0,   1},
        {0,   0.5, 0,   1},
        {0,   0,   0.1, 1},
        {0.2, 0.3, 0.1, 1},
        {0,   0.5, 0.5, 1},
        {1,   0.1, 0.3, 1},
};

//void SetMaterial(GLfloat mat[4]) {
//    int i;
//    for (i = 0; i < 4; i++)
//        curMat[i] = mat[i];
//}

NODE *MarchingCube(float ***dataset, float isoValue, int maxX, int maxY, int maxZ, int current_index) {
    int i, j, k, m, num;
    int inc;
    if (current_index == selected_index) {
        inc = 1;
    } else {
        inc = 2;
    }
    int numOfTriangles;
    float d, norm[3], dx1, dx2, dy1, dy2, dz1, dz2;
    XYZ n[3];
    GRIDCELL v;
    TRIANGLE t[5];
    NODE *list = NULL;
    NODE *tmpNode;
//    printf("isovalue = %f,  maxX = %d, maxY = %d, maxZ = %d\n", isoValue, maxX, maxY, maxZ);
    //x = clock();
    for (i = 0; i < maxX - inc; i += inc) {
        for (j = 0; j < maxY - inc; j += inc) {
            for (k = 0; k < maxZ - inc; k += inc) {
                v.val[0] = dataset[i][j][k];
                v.p[0].x = i * 1.0;
                v.p[0].y = j * 1.0;
                v.p[0].z = k * 1.0;

                v.val[1] = dataset[i + inc][j][k];
                v.p[1].x = i + inc * 1.0;
                v.p[1].y = j * 1.0;
                v.p[1].z = k * 1.0;

                v.val[2] = dataset[i + inc][j + inc][k];
                v.p[2].x = i + inc * 1.0;
                v.p[2].y = j + inc * 1.0;
                v.p[2].z = k * 1.0;

                v.val[3] = dataset[i][j + inc][k];
                v.p[3].x = i * 1.0;
                v.p[3].y = j + inc * 1.0;
                v.p[3].z = k * 1.0;

                v.val[4] = dataset[i][j][k + inc];
                v.p[4].x = i * 1.0;
                v.p[4].y = j * 1.0;
                v.p[4].z = k + inc * 1.0;

                v.val[5] = dataset[i + inc][j][k + inc];
                v.p[5].x = i + inc * 1.0;
                v.p[5].y = j * 1.0;
                v.p[5].z = k + inc * 1.0;

                v.val[6] = dataset[i + inc][j + inc][k + inc];
                v.p[6].x = i + inc * 1.0;
                v.p[6].y = j + inc * 1.0;
                v.p[6].z = k + inc * 1.0;

                v.val[7] = dataset[i][j + inc][k + inc];
                v.p[7].x = i * 1.0;
                v.p[7].y = j + inc * 1.0;
                v.p[7].z = k + inc * 1.0;

                numOfTriangles = Polygonise(v, isoValue, t);
                //printf(" Number of Triangles = %d \n", numOfTriangles);
                for (m = 0; m < numOfTriangles; m++) {
                    dx1 = t[m].p[1].x - t[m].p[0].x;
                    dy1 = t[m].p[1].y - t[m].p[0].y;
                    dz1 = t[m].p[1].z - t[m].p[0].z;

                    dx2 = t[m].p[2].x - t[m].p[0].x;
                    dy2 = t[m].p[2].y - t[m].p[0].y;
                    dz2 = t[m].p[2].z - t[m].p[0].z;

                    norm[0] = dy1 * dz2 - dz1 * dy2;
                    norm[1] = dz1 * dx2 - dx1 * dz2;
                    norm[2] = dx1 * dy2 - dy1 * dx2;
                    d = sqrt(norm[0] * norm[0] + norm[1] * norm[1] + norm[2] * norm[2]);
                    norm[0] /= d;
                    norm[1] /= d;
                    norm[2] /= d;
                    for (num = 0; num < 3; num++) {
                        n[num].x = norm[0];
                        n[num].y = norm[1];
                        n[num].z = norm[2];
                    }
                    tmpNode = (NODE *) malloc(sizeof(NODE));
                    for (num = 0; num < 4; num++) {
                        tmpNode->t.p[num].x = t[m].p[num].x / maxX;
                        tmpNode->t.p[num].y = t[m].p[num].y / maxY;
                        tmpNode->t.p[num].z = t[m].p[num].z / maxZ;
                        tmpNode->n[num] = n[num];
                    }
                    tmpNode->depth = GetDepth(t[m]);
                    for (num = 0; num < 3; num++)
                        tmpNode->mat[num] = materials[current_index][num];
                    tmpNode->next = NULL;
                    tmpNode->sibling = NULL;
                    //printf("Calling insert\n");
                    list = Insert(list, tmpNode);
                }
            }
        }
    }
    //time_in_seconds = (double) (clock() - x)/CLOCKS_PER_SEC;
    //printf("%f", time_in_seconds);
    return list;
}

NODE *Insert(NODE *listset, NODE *tmpNode) {
    NODE *temp;
    if (listset == NULL) return tmpNode;
    else {
        tmpNode->next = listset->next;
        listset->next = tmpNode;
        return listset;
    }
    temp = listset;
    if (temp->next == NULL) {
        if (temp->depth > tmpNode->depth) temp->next = tmpNode;
        else {
            tmpNode->next = temp;
            listset = tmpNode;
        }
        return listset;
    }

    while (temp->next != NULL) {
        if (temp->next->depth > tmpNode->depth) temp = temp->next;
        else {
            tmpNode->next = temp->next;
            temp->next = tmpNode;
            return listset;
        }
    }
    temp->next = tmpNode;
    return listset;

}

float GetDepth(TRIANGLE t) {
    float z;
    z = t.p[0].z;
    z = t.p[1].z > z ? t.p[1].z : z;
    z = t.p[2].z > z ? t.p[2].z : z;
    return z;
}

void DeleteList(NODE *listset) {
    NODE *head = listset;
    NODE *temp;
    NODE *siblingHead;
    while (head->sibling != NULL) {
        siblingHead = head->sibling;
        head->sibling = head->sibling->sibling;
        while (siblingHead != NULL) {
            temp = siblingHead->next;
            free(siblingHead);
            siblingHead = temp;
        }
    }
    while (head != NULL) {
        temp = head->next;
        free(head);
        head = temp;
    }
}

void DrawIsoSurface(NODE *listset) {
    int i;
    XYZ n, p;
    NODE *temp;
    temp = listset;

    glPushMatrix();

    glBegin(GL_TRIANGLES);
    while (temp != NULL) {
//        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, temp->mat);
//        glMaterialfv(GL_FRONT_AND_BACK, GL_COLOR_INDEXES, temp->mat);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, temp->mat);
        for (i = 0; i < 3; i++) {
            n = temp->n[i];
            p = temp->t.p[i];
            glNormal3f(n.x, n.y, n.z);
            glVertex3f(p.x, p.y, p.z);
        }
        temp = temp->next;
    }
    glEnd();

    glPopMatrix();
}

/*
	Source: http://astronomy.swin.edu.au/~pbourke/modelling/polygonise/
	Written by Paul Bourke
	May 1997 
   Given a grid cell and an isolevel, calculate the triangular
   facets required to represent the isosurface through the cell.
   Return the number of triangular facets, the array "triangles"
   will be loaded up with the vertices at most 5 triangular facets.
	0 will be returned if the grid cell is either totally above
   of totally below the isolevel.
*/
int Polygonise(GRIDCELL grid, float isolevel, TRIANGLE *triangles) {
    int i, ntriang;
    int cubeindex;
    XYZ vertlist[12];
    /*
       Determine the index into the edge table which
       tells us which vertices are inside of the surface
    */
    cubeindex = 0;
    if (grid.val[0] < isolevel) cubeindex |= 1;
    if (grid.val[1] < isolevel) cubeindex |= 2;
    if (grid.val[2] < isolevel) cubeindex |= 4;
    if (grid.val[3] < isolevel) cubeindex |= 8;
    if (grid.val[4] < isolevel) cubeindex |= 16;
    if (grid.val[5] < isolevel) cubeindex |= 32;
    if (grid.val[6] < isolevel) cubeindex |= 64;
    if (grid.val[7] < isolevel) cubeindex |= 128;

    /* Cube is entirely in/out of the surface */
    if (edgeTable[cubeindex] == 0)
        return (0);

    /* Find the vertices where the surface intersects the cube */
    if (edgeTable[cubeindex] & 1)
        vertlist[0] =
                VertexInterp(isolevel, grid.p[0], grid.p[1], grid.val[0], grid.val[1]);
    if (edgeTable[cubeindex] & 2)
        vertlist[1] =
                VertexInterp(isolevel, grid.p[1], grid.p[2], grid.val[1], grid.val[2]);
    if (edgeTable[cubeindex] & 4)
        vertlist[2] =
                VertexInterp(isolevel, grid.p[2], grid.p[3], grid.val[2], grid.val[3]);
    if (edgeTable[cubeindex] & 8)
        vertlist[3] =
                VertexInterp(isolevel, grid.p[3], grid.p[0], grid.val[3], grid.val[0]);
    if (edgeTable[cubeindex] & 16)
        vertlist[4] =
                VertexInterp(isolevel, grid.p[4], grid.p[5], grid.val[4], grid.val[5]);
    if (edgeTable[cubeindex] & 32)
        vertlist[5] =
                VertexInterp(isolevel, grid.p[5], grid.p[6], grid.val[5], grid.val[6]);
    if (edgeTable[cubeindex] & 64)
        vertlist[6] =
                VertexInterp(isolevel, grid.p[6], grid.p[7], grid.val[6], grid.val[7]);
    if (edgeTable[cubeindex] & 128)
        vertlist[7] =
                VertexInterp(isolevel, grid.p[7], grid.p[4], grid.val[7], grid.val[4]);
    if (edgeTable[cubeindex] & 256)
        vertlist[8] =
                VertexInterp(isolevel, grid.p[0], grid.p[4], grid.val[0], grid.val[4]);
    if (edgeTable[cubeindex] & 512)
        vertlist[9] =
                VertexInterp(isolevel, grid.p[1], grid.p[5], grid.val[1], grid.val[5]);
    if (edgeTable[cubeindex] & 1024)
        vertlist[10] =
                VertexInterp(isolevel, grid.p[2], grid.p[6], grid.val[2], grid.val[6]);
    if (edgeTable[cubeindex] & 2048)
        vertlist[11] =
                VertexInterp(isolevel, grid.p[3], grid.p[7], grid.val[3], grid.val[7]);

    /* Create the triangle */
    ntriang = 0;
    for (i = 0; triTable[cubeindex][i] != -1; i += 3) {
        triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i]];
        triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i + 1]];
        triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i + 2]];
        ntriang++;
    }

    return (ntriang);
}

/*
   Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
*/
XYZ VertexInterp(float isolevel, XYZ p1, XYZ p2, float valp1, float valp2) {
    float mu;
    XYZ p;

    if (fabs(isolevel - valp1) < 0.00001)
        return (p1);
    if (fabs(isolevel - valp2) < 0.00001)
        return (p2);
    if (fabs(valp1 - valp2) < 0.00001)
        return (p1);
    mu = (isolevel - valp1) / (valp2 - valp1);
    p.x = p1.x + mu * (p2.x - p1.x);
    p.y = p1.y + mu * (p2.y - p1.y);
    p.z = p1.z + mu * (p2.z - p1.z);

    return (p);
}

// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char) *str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

int takeInput(char *str) {
    char *found;
    int i = 0;
    while ((found = strsep(&str, ",")) != NULL && i < MAX_ISOVALUE) {
        found = trimwhitespace(found);
//        printf("found %s\n", found);
        double value = strtod(found, NULL);
        if (value != 0) {
//            printf("found %lf\n", value);
            *(isovalueArray + i) = value;
            i++;
        }
    }
    isovalue_len = i;
    return i > 0 ? 0 : 1;
}

void test(int testSize) {
    isovalueArray = malloc(testSize * sizeof(double));
    for (int i = 0; i < testSize; ++i) {
        *(isovalueArray + i) = 0.02;
    }
    isovalue_len = testSize;
}

int interaction() {
    char input[MAX_ENTER];
    isovalueArray = calloc(MAX_ISOVALUE, sizeof(double));
    while (1) {
        printf("Please enter the isovalues(up to %d) between 0.01 and 5.2 separated by a comma(e.g. 0.01,1.03): ",
               MAX_ISOVALUE);
        fgets(input, MAX_ENTER, stdin);
        if (takeInput(input)) {
            continue;
        }
        break;
    }
}

int main(int argc, char **argv) {

    interaction();
//    test(9);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1200, 1200);
    glutInitWindowPosition(0, 0);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseClick);
    glutReshapeFunc(reshape);

    x = clock();
    memset(rot, 0, 3 * 4);
    memset(trans, 0, 3 * 4);
    trans[2] = -4.0;

    flag = 0;
    loaddata();

    time_in_seconds = (double) (clock() - x) / CLOCKS_PER_SEC;
    printf("Loading data %f\n", time_in_seconds);
    x = clock();

    loadlist();

    time_in_seconds = (double) (clock() - x) / CLOCKS_PER_SEC;
    printf("Creating list %f\n", time_in_seconds);
    x = clock();

    glutMainLoop();
    free(isovalueArray);
    return 0;
}


void Draw(NODE *list, int i, int j) {
    int hi = 1, wi = 1;
//    glViewport(j * g_w / wi, i * g_h / hi, 1 * g_w / wi, 1 * g_h / hi);
    NODE *head = list;
    GLsizei segment_w, segment_h;

    switch (isovalue_len) {
        case 1:
            segment_w = g_w;
            segment_h = g_h;
            break;
        case 2:
            segment_w = (GLsizei) (g_w * 0.5);
            segment_h = g_h;
            break;
        case 3:
        case 4:
            segment_w = (GLsizei) (g_w * 0.5);
            segment_h = (GLsizei) (g_h * 0.5);
            break;
        case 5:
        case 6:
            segment_w = (GLsizei) (g_w / 3);
            segment_h = (GLsizei) (g_h / 2);
            break;
        case 7:
        case 8:
        case 9:
            segment_w = (GLsizei) (g_w / 3);
            segment_h = (GLsizei) (g_h / 3);
            break;
        default:
            perror("invalid isovalue_len");
            return;
    }
    int k = 0;
    while (head != NULL) {
        if (isovalue_len == 1) {
            glViewport(0, 0, segment_w, segment_h);
        } else if (isovalue_len == 2) {
            glViewport(k * segment_w, 0, segment_w, segment_h);
        } else if (isovalue_len <= 4) {
            glViewport((k % 2) * segment_w, (1 - k / 2) * segment_h, segment_w, segment_h);
        } else if (isovalue_len <= 6) {
            glViewport((k % 3) * segment_w, (1 - k / 3) * segment_h, segment_w, segment_h);
        } else if (isovalue_len <= 9) {
            glViewport((k % 3) * segment_w, (2 - k / 3) * segment_h, segment_w, segment_h);
        }
        glPushMatrix();
        glTranslatef((GLfloat) -0.5, (GLfloat) -0.5, (GLfloat) -0.5);
        DrawIsoSurface(head);
        glPopMatrix();
        glutWireCube(1);
        head = head->sibling;
        k++;
    }
}

long diff;
clock_t start, end;
int counter = 0;

void display(void) {
    x = clock();
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glRotatef(angle, 1.0, 1.0, 1.0);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material);
    Draw(list1, 0, 0);
    glutSwapBuffers();
    time_in_seconds = (double) (clock() - x) / CLOCKS_PER_SEC;
//    printf("Display %f\n", time_in_seconds);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {

    switch (key) {
        case 27:
            clearlist();
            exit(0);
            break;
        case 'a':
            for (int i = 0; i < isovalue_len; ++i) {
                *(isovalueArray + i) = *(isovalueArray + i) + 0.02;
                printf("\n%d: iso value %f\n", i + 1, *(isovalueArray + i));
            }
//            isoval = isoval + 0.02;
//            printf("\niso value %f\n", isoval);
            x = clock();
            clearlist();
            time_in_seconds = (double) (clock() - x) / CLOCKS_PER_SEC;
            printf("Clearing List %f\n", time_in_seconds);
            x = clock();
            loadlist();
            time_in_seconds = (double) (clock() - x) / CLOCKS_PER_SEC;
            printf("Creating list1 %f\n", time_in_seconds);

            glutPostRedisplay();
            break;
        case 's':
            for (int i = 0; i < isovalue_len; ++i) {
                *(isovalueArray + i) = *(isovalueArray + i) - 0.02;
                printf("\n%d: iso value %f\n", i + 1, *(isovalueArray + i));
            }
//            isoval = isoval - 0.02;
//            printf("\niso value %f\n", isoval);
            clearlist();
            time_in_seconds = (double) (clock() - x) / CLOCKS_PER_SEC;
            printf("Clearing List %f\n", time_in_seconds);
            x = clock();
            loadlist();
            time_in_seconds = (double) (clock() - x) / CLOCKS_PER_SEC;
            printf("Interactivity%f\n", time_in_seconds);
            glutPostRedisplay();
            break;
        case 'l':
            angle = angle + 1;
            break;
        case 'k':
            angle = angle - 1;
            break;

    }
}

void mouseClick(int button, int state, int x, int y) {
    int segment_w, segment_h, n_h, n_w;
    switch (button) {
        case GLUT_LEFT_BUTTON:
            switch (state) {
                case GLUT_DOWN:
                    switch (isovalue_len) {
                        case 1:
                            segment_w = g_w;
                            segment_h = g_h;
                            break;
                        case 2:
                            segment_w = (int) (g_w * 0.5);
                            segment_h = g_h;
                            break;
                        case 3:
                        case 4:
                            segment_w = (int) (g_w * 0.5);
                            segment_h = (int) (g_h * 0.5);
                            break;
                        case 5:
                        case 6:
                            segment_w = (int) (g_w / 3);
                            segment_h = (int) (g_h / 2);
                            break;
                        case 7:
                        case 8:
                        case 9:
                            segment_w = (int) (g_w / 3);
                            segment_h = (int) (g_h / 3);
                            break;
                        default:
                            perror("invalid isovalue_len");
                            return;
                    }
                    n_h = y / segment_h;
                    n_w = x / segment_w;
                    switch (isovalue_len) {
                        case 1:
                            selected_index = 0;
                            break;
                        case 2:
                            selected_index = n_w;
                            break;
                        case 3:
                        case 4:
                            selected_index = 2 * n_h + n_w;
                            break;
                        case 5:
                        case 6:
                        case 7:
                        case 8:
                        case 9:
                            selected_index = 3 * n_h + n_w;
                            break;
                        default:
                            perror("invalid isovalue_len");
                            return;
                    }
//                    printf("x: %d, y: %d, select_index: %d\n", x, y, selected_index);
                    x = clock();
                    clearlist();
                    time_in_seconds = (double) (clock() - x) / CLOCKS_PER_SEC;
                    printf("Clearing List %f\n", time_in_seconds);
                    x = clock();
                    loadlist();
                    time_in_seconds = (double) (clock() - x) / CLOCKS_PER_SEC;
                    printf("Creating list1 %f\n", time_in_seconds);

                    glutPostRedisplay();
                    break;
                case GLUT_UP:
                    break;
                default:
                    break;
            }
            break;
        case GLUT_RIGHT_BUTTON:
            break;
        default:
            break;
    }
}


void reshape(int w, int h) {
    g_w = w;
    g_h = h;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.75, 0.75, -0.75, 0.75, -10, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void init(void) {
//    GLfloat material[] = {0.0, 0, 0.5};
    static GLfloat light_position0[] = {1.0, 1.0, 1.0, 1.0};
    static GLfloat light_ambient[] = {0.0, 0.0, 1.0, 0.0};
    static GLfloat light_position1[] = {1.0, 0.0, 2.0, 1.0};
    static GLfloat light_specular[] = {0.0, 0.0, 1.0, 0.0};
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClearDepth(1.0f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
//    SetMaterial(material);

    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);
}

float ***Load(const char *fileName, float ***chargeDensity) {
    int index = 0, i, j, k;
    float volume, latticeConstant, chargeDen, kPoints[3];
    FILE *fp;
    fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("Error %d \n", errno);
    }
    //printf("Reading a chunk from the charge density file: %s\n", fileName);
    fscanf(fp, "%f", &latticeConstant);
    volume = latticeConstant * latticeConstant * latticeConstant;
    for (j = 0; j < 3; j++)
        fscanf(fp, "%f", &kPoints[j]);
    volume = latticeConstant * latticeConstant * latticeConstant * kPoints[0] * kPoints[0] * kPoints[0];
    fscanf(fp, "%d %d %d", &nx, &ny, &nz);
    //width = gridX; height = gridY; depth = gridZ;
    chargeDensity = (float ***) malloc(sizeof(float **) * nx);
    for (i = 0; i < nx; i++) {
        chargeDensity[i] = (float **) malloc(sizeof(float *) * ny);
        for (j = 0; j < ny; j++) {
            chargeDensity[i][j] = (float *) malloc(sizeof(float) * nz);
        }
    }
    for (i = 0; i < nz; i++)
        for (j = 0; j < ny; j++)
            for (k = 0; k < nx; k++) {
                fscanf(fp, "%f", &chargeDensity[k][j][i]);
                chargeDensity[k][j][i] /= volume;
            }
    fclose(fp);
    return (chargeDensity);
}

void freedata(float ***rho) {
    int i, j;
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++)
            free(rho[i][j]);
        free(rho[i]);
    }
    free(rho);
}

void callfree() {
    freedata(data1);
    data1 = NULL;
}


void loaddata() {
    data1 = Load(file[0], data1);
}

void loadlist() {
    NODE *tmpNode;
    for (int i = 0; i < isovalue_len; i++) {
        tmpNode = MarchingCube(data1, (float) *(isovalueArray + i), nx, ny, nz, i);
        if (list1 == NULL) {
            list1 = tmpNode;
        } else {
            NODE *tmpHead = list1;
            while (tmpHead->sibling != NULL) {
                tmpHead = tmpHead->sibling;
            }
            tmpHead->sibling = tmpNode;
        }
    }
}

void clearlist() {
    DeleteList(list1);
    list1 = NULL;
}

