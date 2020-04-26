#include "serialization.h"

// === SERIALIZATION === //

unsigned char * serialize_int(unsigned char *buffer, int value) {
  /* Write big-endian int value into buffer; assumes 32-bit int and 8-bit char. */
  buffer[0] = value >> 24;
  buffer[1] = value >> 16;
  buffer[2] = value >> 8;
  buffer[3] = value;

  return buffer + 4;
}

unsigned char * serialize_char(unsigned char *buffer, char value) {
  buffer[0] = value;

  return buffer + 1;
}

unsigned char * serialize_string(unsigned char *buffer, char *value) {
  for (int i = 0; i < strlen(value); i++) {
    buffer[i] = value[i];
  }

  return buffer + strlen(value);
}

// In case of if this is needed.
unsigned char * serialize_window(unsigned char *buffer, WINDOW value) {
  int window_len = sizeof(WINDOW);
  // how could I read the whole window struct and pass it to socket ?
  // ...
  return buffer;
}

unsigned char * serialize_player(unsigned char *buffer, Player_t *p) {
  buffer = serialize_int(buffer, p->pid);
  buffer = serialize_int(buffer, p->xLoc);
  buffer = serialize_int(buffer, p->yLoc);
  buffer = serialize_int(buffer, p->xMax);
  buffer = serialize_int(buffer, p->yMax);
  buffer = serialize_char(buffer, p->character);

  return buffer;
}

int send_player(int socket, Player_t *p) {
  unsigned char buffer[BUFFER_SIZE], *ptr;

  ptr = serialize_player(buffer, p);

  // Already checking if sending message was successful.
  // return sendto(socket, buffer, ptr - buffer, 0, dest, dlen) == ptr - buffer;
  return send(socket, buffer, ptr - buffer, 0) == ptr - buffer;
}

// === DESERIALIZATION === //

// More correct ???
// int deserialize_temp(unsigned char *buffer, struct temp *value);
unsigned char * deserialize_int(unsigned char *buffer, int *value) {
    unsigned char number_s[sizeof(int)];
    int number_i;

    memcpy(number_s, buffer, sizeof(int));
    *value = *(int*)number_s;

    return buffer + 4;
}

unsigned char * deserialize_char(unsigned char *buffer, char *value) {
    *value = buffer[0];

    return buffer + 1;
}

void deserialize_player(unsigned char *buffer, Player_t *p) {
  buffer = deserialize_int(buffer, &(p->pid));
  buffer = deserialize_int(buffer, &(p->xLoc));
  buffer = deserialize_int(buffer, &(p->yLoc));
  buffer = deserialize_int(buffer, &(p->xMax));
  buffer = deserialize_int(buffer, &(p->yMax));
  buffer = deserialize_char(buffer, &(p->character));
}