
void sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}

size_t getFileSize(FILE *fp){
    size_t file_size = 0;

    // Get File Size
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return file_size;
}

char* strtolower(char* text){
    int i = 0, j = strlen(text);
    char* buffer;
    buffer = malloc(j+1);

    for(i = 0; i <= j; i++){
        if(text[i] > 64 && text[i] < 91){
            buffer[i] = text[i] + 32;
        }else{
            buffer[i] = text[i];
        }
    }
    return buffer;
}

char* strtoupper(char* text){
    int i = 0, j = strlen(text);
    char* buffer;
    buffer = malloc(j+1);

    for(i = 0; i <= j; i++){
        if(text[i] > 96 && text[i] < 122){
            buffer[i] = text[i] - 32;
        }else{
            buffer[i] = text[i];
        }
    }
    return buffer;
}

/*
    String Replace Function
    Source: http://stackoverflow.com/a/32413923
*/

void str_replace(char *target, const char *needle, const char *replacement){
    char buffer[1024] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (1) {
        const char *p = strstr(tmp, needle);

        // walked past last occurrence of needle; copy remaining part
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        // copy part before needle
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        // copy replacement string
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        // adjust pointers, move on
        tmp = p + needle_len;
    }

    // write altered string back to target
    strcpy(target, buffer);
}

/*
    URL Decoding
    Source: https://www.rosettacode.org/wiki/URL_decoding#C
*/

int ishex(int x){
	return	(x >= '0' && x <= '9')	||
		(x >= 'a' && x <= 'f')	||
		(x >= 'A' && x <= 'F');
}

int url_decode(const char *s, char *dec){
	char *o;
	const char *end = s + strlen(s);
	int c;

	for (o = dec; s <= end; o++) {
		c = *s++;
		if (c == '+') c = ' ';
		else if (c == '%' && (	!ishex(*s++)	||
					!ishex(*s++)	||
					!sscanf(s - 2, "%2x", &c)))
			return -1;

		if (dec) *o = c;
	}

	return o - dec;
}

/* https://stackoverflow.com/a/40116476 */
unsigned long ToUInt(char* str)
{
    unsigned long mult = 1;
    unsigned long re = 0;
    int len = strlen(str);
    for(int i = len -1 ; i >= 0 ; i--)
    {
        re = re + ((int)str[i] -48)*mult;
        mult = mult*10;
    }
    return re;
}


/*
    Generating RFC 1123 Time Stamp
    Source: http://stackoverflow.com/a/2727122
*/

/*
static const char *DAY_NAMES[] =
  { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static const char *MONTH_NAMES[] =
  { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

char *Rfc1123_DateTime(time_t t){
    const int RFC1123_TIME_LEN = 29;
    struct tm tm;
    char * buf = malloc(RFC1123_TIME_LEN+1);

    gmtime_s(&tm, &t);

    strftime(buf, RFC1123_TIME_LEN+1, "---, %d --- %Y %H:%M:%S GMT", &tm);
    memcpy(buf, DAY_NAMES[tm.tm_wday], 3);
    memcpy(buf+8, MONTH_NAMES[tm.tm_mon], 3);

    return buf;
}
*/
