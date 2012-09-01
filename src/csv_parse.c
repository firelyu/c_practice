#include    "csv_parse.h"

/*
 * reset: set variables back to the beginning.
 */
void reset() {
    free(line);
    free(sline);
    free(fields);
    line = NULL;
    sline = NULL;
    fields = NULL;
    maxlinesize = maxfieldsize = nfield = 0;
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
    unsigned int    pre, head;

    for (pre = head = 0; str[head] != '\0'; pre++, head++) {
        if (str[head] == '"' && str[++head] != '"') {
            // copy up to next separator or '\0'
            size_t k = strcspn((char *) (str + head), (char *) fieldsep);
            memmove((char *) (str + pre), (char *) (str + head), k);
            pre += k;
            head += k;
            break;
        }
        str[pre] = str[head];
    }
    str[pre] = '\0';

    return str + head;
}

/*
 * splite: splite line into field.
 */
static int splite() {
    char    *head, **newfield;
    char    *ptmpsep;       // Pointer to temp separator char
    int     tmpsep;         // Temp separator char

    nfield = 0;
    if (line[0] == '\0') return 0;
    strcpy(sline, line);
    head = sline;

    do {
        if (nfield >= maxfieldsize) {
            maxfieldsize *= 2;
            newfield = (char **) realloc(fields, maxfieldsize * sizeof(fields[0]));
            if (newfield == NULL) return NOMEN;
            fields = newfield;
        }
        
        if (*head == '"') ptmpsep = advquoted(++head);
        else ptmpsep = head + strcspn(head, fieldsep);   // ptmpsep point at the ','
        tmpsep = ptmpsep[0];
        ptmpsep[0] = '\0';
        fields[nfield] = head;
        nfield++;
        head = ptmpsep + 1;        // point to next field
    } while (tmpsep == ',');
    
    return nfield;
}

/* 
 * csvgetline: get one line, grow as needed
 * sample input: "LU",86.25."11/4/1998","2:19PM",+4.0625
 */
char* csvgetline(FILE *fin) {
    int     pos, one_char;
    char    *newline, *newsline;

    if (line == NULL) {
        maxlinesize = maxfieldsize = INIT_LENGTH;
        line = (char *) malloc(maxlinesize * sizeof(line[0]));
        sline = (char *) malloc(maxlinesize * sizeof(line[0]));
        fields = (char **) malloc(maxfieldsize * sizeof(fields[0]));
        if (line == NULL || sline == NULL || fields == NULL) {
            reset();
            return NULL;
        }
    }
    
    for (pos = 0; (one_char = getc(fin)) != EOF && !endofline(fin, one_char); pos++) {
        if (pos > maxlinesize - 1) {
            maxlinesize *= 2;
            newline = (char *) realloc(line, maxlinesize * sizeof(newline[0]));
            newsline = (char *) realloc(sline, maxlinesize * sizeof(newsline[0]));
            if (newline == NULL || newsline == NULL) {
                reset();
                return NULL;
            }
            line = newline;
            sline = newsline;
        }
        line[pos] = one_char;
    }
    line[pos] = '\0';

    if (splite() == NOMEN) {
        reset();
        return NULL;
    }

    return (one_char == EOF && pos == 0) ? NULL : line;
}

/*
 * csvfield: return pointer to n-th field
 */
char* csvfield(int n) {
    if (n < 0 || n >= nfield) return NULL;
    
    return fields[n];
}

/*
 * csvnfield: return number of field
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
            printf("fields[%d] = '%s'\n", i, csvfield(i));
    }
    
    if (fclose(fh) != 0) {
        perror("Close file");
        return 1;
    }

    return 0;
}
