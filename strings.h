#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED


typedef struct
{
  u8 bytes[4];
} utf8;

typedef struct
{
  u32 length;
  utf8 cs[];
} String;



String *
new_string(const char *);

char *
to_c_string(String *);

void
print(String *);

String *
int_to_string(u32);

String *
join(String *, String *);

void
append(String **, String *);


#endif