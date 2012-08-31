#include    "csv_parse.h"

/*
 * reset: set variables back to the beginning.
 */
void reset() {
    free(line);
    free(sline);
    free(field);
    line = NULL;
    sline = NULL;
    field = NULL;
    maxline = maxfield = nfield = 0;
}

/*
 * endofline: check for and consume \r, \n, \r\n or EOF
 */
static int endofline(FILE *fin, int c) {
    int     is_eol;

    is_eol = (c == '\r' || c == '\n');
    if (c == '\r') {
        c = getc(fin);
        // read too far; put c back
        if (c != '\n' && c != EOF) ungetc(c, fin);
    }

    return is_eol;
}

/*
 * advquoted: quoted field: return pointer to next separator
 */
static char* advquoted(char *str) {
    int     i, j;

    for (i = j = 0; str[j] != '\0'; i++, j++) {
        if (str[j] == '"' && str[++j] != '"') {
            // copy up to next separator or '\0'
            int k = strcspn((char *) (str + j), fieldsep);
            memmove((char *) (str + i), (char *) (str + j), k);
            i += k;
            j += k;
            break;
        }
        str[i] = str[j];
    }
    str[i] = '\0';

    return str + j;
}

/*
 * splite: splite line into field.
 */
static int splite() {
    char    *p, **newfield;
    char    *ptmpsep;       // Pointer to temp separator char
    int     tmpsep;         // Temp separator char

    nfield = 0;
    if (line[0] == '\0') return 0;
    strcpy(sline, line);
    p = sline;

    do {
        if (nfield >= maxfield) {
            maxfield *= 2;
            newfield = (char **) realloc(field, maxfield * sizeof(field[0]));
            if (newfield == NULL) return NOMEN;
            field = newfield;
        }
        
        if (*p == '"') ptmpsep = advquoted(++p);
        else ptmpsep = p + strcspn(p, fieldsep);   // ptmpsep point at the ','
        tmpsep = ptmpsep[0];
        ptmpsep[0] = '\0';
        field[nfield] = p;
        nfield++;
        p = ptmpsep + 1;        // point to next field
    } while (tmpsep == ',');
    
    return nfield;
}

/* 
 * csvgetline: get one line, grow as needed
 * sample input: "LU",86.25."11/4/1998","2:19PM",+4.0625
 */
char* csvgetline(FILE *fin) {
    int     i, one_char;
    char    *newline, *newsline;

    if (line == NULL) {
        maxline = maxfield = INIT_LENGTH;
        line = (char *) malloc(maxline * sizeof(*line));
        sline = (char *) malloc(maxline * sizeof(*line));
        field = (char **) malloc(maxfield * sizeof(*field));
        if (line == NULL || sline == NULL || field == NULL) {
            reset();
            return NULL;
        }
    }
    
    for (i = 0; (one_char = getc(fin)) != EOF && !endofline(fin, one_char); i++) {
        if (i > maxline - 1) {
            maxline *= 2;
            newline = (char *) realloc(line, maxline);
            newsline = (char *) realloc(sline, maxline);
            if (newline == NULL || newsline == NULL) {
                reset();
                return NULL;
            }
            line = newline;
            sline = newsline;
        }
        line[i] = one_char;
    }
    line[i] = '\0';

    if (splite() == NOMEN) {
        reset();
        return NULL;
    }

    return (one_char == EOF && i == 0) ? NULL : line;
}

/*
 * csvfield: return pointer to n-th field
 */
char* csvfield(int n) {
    if (n < 0 || n >= nfield) return NULL;
    
    return field[n];
}

/*
 * csvnfield: return number of fields
 */
int csvnfield() {
    return nfield;
}

void usage() {
    printf("csv_parse <file_name>\n");
    printf("<file_name> : The csv file name.\n");
    printf("\n");
}

int main(int argc, char *argv[]) {
    int     i;
    char    *line, *file_name;
    FILE    *fh;
    
    if (argc != 2) {
        usage();
        return 1;
    }

    file_name = argv[1];
    fh = fopen(file_name, "r");
    if (fh == NULL) {
        perror("Open file");
        return 1;
    }

    while ((line = csvgetline(fh)) != NULL) {
        printf("line = '%s'\n", line);
        for (i = 0; i < csvnfield(); i++)
            printf("field[%d] = '%s'\n", i, csvfield(i));
    }
    
    if (fclose(fh) != 0) {
        perror("Close file");
        return 1;
    }

    return 0;
}
