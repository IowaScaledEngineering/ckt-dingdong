#include <avr/pgmspace.h>

const uint8_t start_wav[] PROGMEM = {
  0x80, 0x85, 0x77, 0x7e, 0x8c, 0x7c, 0x76, 0x82, 0x88, 0x76, 0x87, 0x8c,
  0x61, 0x77, 0xa3, 0x93, 0x60, 0x63, 0x8f, 0xa0, 0x86, 0x60, 0x6e, 0x92,
  0x98, 0x77, 0x69, 0x81, 0x90, 0x83, 0x74, 0x7b, 0x86, 0x86, 0x7a, 0x7b,
  0x83, 0x85, 0x7e, 0x7b, 0x7f, 0x83, 0x7a, 0x86, 0x8d, 0x66, 0x79, 0x9c,
  0x7b, 0x71, 0x87, 0x81, 0x7c, 0x82, 0x83, 0x7a, 0x7c, 0x8a, 0x7e, 0x79,
  0x7f, 0x87, 0x7f, 0x79, 0x81, 0x82, 0x7c, 0x84, 0x8e, 0x67, 0x6c, 0x9b,
  0x9f, 0x6c, 0x55, 0x86, 0xa6, 0x8b, 0x61, 0x6c, 0x8e, 0x99, 0x7c, 0x68,
  0x7d, 0x91, 0x86, 0x73, 0x79, 0x87, 0x87, 0x7b, 0x78, 0x82, 0x86, 0x81,
  0x79, 0x7c, 0x80, 0x87, 0x8f, 0x68, 0x72, 0x9e, 0x7f, 0x6d, 0x88, 0x80,
  0x7a, 0x88, 0x81, 0x76, 0x7c, 0x8f, 0x7e, 0x74, 0x84, 0x84, 0x7c, 0x80,
  0x82, 0x7a, 0x7f, 0x87, 0x77, 0x8c, 0x84, 0x5b, 0x84, 0xa8, 0x89, 0x55,
  0x6a, 0x9f, 0x9e, 0x72, 0x60, 0x7e, 0x99, 0x8d, 0x6b, 0x73, 0x8d, 0x88,
  0x78, 0x7a, 0x82, 0x85, 0x7f, 0x78, 0x81, 0x87, 0x80, 0x7c, 0x7a, 0x80,
  0x84, 0x91, 0x73, 0x63, 0x98, 0x91, 0x68, 0x7e, 0x8c, 0x75, 0x82, 0x87,
  0x7c, 0x78, 0x84, 0x86, 0x78, 0x7f, 0x81, 0x7e, 0x80, 0x82, 0x7f, 0x7a,
  0x83, 0x84, 0x7e, 0x7c, 0x80, 0x80, 0x7d, 0x96, 0x73, 0x58, 0x97, 0xaa,
  0x77, 0x4e, 0x79, 0xac, 0x92, 0x60, 0x68, 0x90, 0x96, 0x79, 0x6d, 0x82,
  0x8b, 0x81, 0x76, 0x7f, 0x89, 0x80, 0x75, 0x7d, 0x8b, 0x84, 0x77, 0x79,
  0x7e, 0x87, 0x96, 0x69, 0x68, 0x9c, 0x88, 0x6f, 0x85, 0x7d, 0x77, 0x8d,
  0x81, 0x77, 0x7c, 0x88, 0x81, 0x77, 0x85, 0x7f, 0x7b, 0x83, 0x80, 0x7c,
  0x82, 0x83, 0x78, 0x80, 0x86, 0x7f, 0x75, 0x8d, 0x90, 0x54, 0x7b, 0xae,
  0x8d, 0x55, 0x62, 0xa5, 0x9f, 0x6d, 0x5e, 0x85, 0x9b, 0x83, 0x6a, 0x7a,
  0x8d, 0x85, 0x75, 0x79, 0x89, 0x86, 0x78, 0x76, 0x86, 0x8a, 0x7b, 0x76,
  0x7f, 0x7f, 0x97, 0x79, 0x5b, 0x95, 0x94, 0x6c, 0x81, 0x85, 0x72, 0x8a,
  0x8a, 0x74, 0x77, 0x8c, 0x84, 0x74, 0x85, 0x83, 0x78, 0x81, 0x86, 0x7e,
  0x7b, 0x81, 0x83, 0x7e, 0x80, 0x7f, 0x80, 0x81, 0x81, 0x76, 0x91, 0x88,
  0x50, 0x88, 0xad, 0x85, 0x4e, 0x70, 0xae, 0x8d, 0x66, 0x6e, 0x8d, 0x8d,
  0x7c, 0x74, 0x83, 0x89, 0x78, 0x7b, 0x86, 0x84, 0x79, 0x7c, 0x81, 0x85,
  0x81, 0x78, 0x7c, 0x80, 0x96, 0x7a, 0x5e, 0x92, 0x92, 0x6d, 0x83, 0x83,
  0x6f, 0x90, 0x87, 0x6f, 0x80, 0x86, 0x82, 0x79, 0x83, 0x80, 0x7c, 0x84,
  0x7e, 0x7f, 0x83, 0x7e, 0x7b, 0x84, 0x84, 0x79, 0x7d, 0x86, 0x7f, 0x7d,
  0x79, 0x94, 0x82, 0x50, 0x8f, 0xad, 0x7f, 0x4b, 0x76, 0xb0, 0x8a, 0x64,
  0x6b, 0x93, 0x90, 0x74, 0x74, 0x8a, 0x85, 0x73, 0x7f, 0x8c, 0x80, 0x72,
  0x7e, 0x87, 0x86, 0x7b, 0x76, 0x7d, 0x84, 0x9a, 0x6b, 0x5f, 0xa1, 0x8a,
  0x69, 0x8a, 0x7d, 0x71, 0x96, 0x81, 0x70, 0x7f, 0x89, 0x81, 0x78, 0x84,
  0x7c, 0x7e, 0x87, 0x7c, 0x7b, 0x83, 0x82, 0x7b, 0x82, 0x82, 0x7b, 0x7f,
  0x83, 0x7e, 0x80, 0x7f, 0x7e, 0x78, 0x93, 0x8b, 0x4b, 0x88, 0xb2, 0x81,
  0x4d, 0x72, 0xb2, 0x85, 0x64, 0x73, 0x94, 0x86, 0x73, 0x7c, 0x8a, 0x81,
  0x6e, 0x84, 0x91, 0x79, 0x6f, 0x87, 0x89, 0x7e, 0x79, 0x7d, 0x7c, 0x93,
  0x84, 0x59, 0x8d, 0x97, 0x70, 0x7d, 0x88, 0x6f, 0x87, 0x91, 0x6a, 0x82,
  0x87, 0x7a, 0x83, 0x81, 0x7c, 0x7d, 0x86, 0x7f, 0x79, 0x85, 0x80, 0x7d,
  0x81, 0x7e, 0x82, 0x80, 0x7f, 0x7d, 0x80, 0x82, 0x7e, 0x7b, 0x83, 0x83,
  0x7e, 0x74, 0x93, 0x8f, 0x48, 0x86, 0xb8, 0x80, 0x44, 0x7a, 0xb5, 0x80,
  0x63, 0x74, 0x96, 0x87, 0x71, 0x7b, 0x8e, 0x7d, 0x70, 0x89, 0x8b, 0x78,
  0x75, 0x84, 0x89, 0x7f, 0x75, 0x80, 0x7e, 0x94, 0x7e, 0x5a, 0x96, 0x90,
  0x68, 0x8c, 0x80, 0x6e, 0x90, 0x83, 0x77, 0x7d, 0x85, 0x7f, 0x7e, 0x87,
  0x76, 0x81, 0x88, 0x77, 0x7c, 0x89, 0x80, 0x77, 0x81, 0x85, 0x7c, 0x7f,
  0x7f, 0x81, 0x7f, 0x81, 0x77, 0x8f, 0x8e, 0x4b, 0x88, 0xb5, 0x7c, 0x48,
  0x7e, 0xb5, 0x77, 0x64, 0x81, 0x92, 0x7a, 0x76, 0x88, 0x86, 0x74, 0x78,
  0x8f, 0x84, 0x6f, 0x80, 0x8c, 0x7e, 0x76, 0x80, 0x7f, 0x82, 0x95, 0x64,
  0x74, 0x9e, 0x79, 0x76, 0x8b, 0x72, 0x7d, 0x97, 0x6f, 0x7c, 0x87, 0x79,
  0x85, 0x81, 0x7c, 0x7c, 0x86, 0x80, 0x79, 0x84, 0x80, 0x7e, 0x81, 0x7c,
  0x82, 0x83, 0x7d, 0x7b, 0x84, 0x81, 0x7d, 0x7e, 0x81, 0x80, 0x80, 0x77,
  0x88, 0x99, 0x4c, 0x76, 0xbd, 0x87, 0x45, 0x71, 0xb8, 0x82, 0x61, 0x78,
  0x95, 0x84, 0x6e, 0x82, 0x91, 0x71, 0x73, 0x93, 0x83, 0x70, 0x7e, 0x8a,
  0x82, 0x78, 0x7b, 0x83, 0x7e, 0x96, 0x6d, 0x66, 0xa3, 0x7d, 0x6e, 0x92,
  0x71, 0x7b, 0x93, 0x77, 0x7a, 0x83, 0x81, 0x7b, 0x88, 0x7b, 0x76, 0x8d,
  0x7f, 0x75, 0x82, 0x88, 0x7b, 0x7c, 0x82, 0x80, 0x80, 0x80, 0x7b, 0x85,
  0x7e, 0x7d, 0x7a, 0x95, 0x82, 0x48, 0x99, 0xb4, 0x6a, 0x48, 0x97, 0xac,
  0x62, 0x6f, 0x93, 0x84, 0x6d, 0x87, 0x8f, 0x76, 0x70, 0x8a, 0x8e, 0x73,
  0x74, 0x8d, 0x87, 0x72, 0x7e, 0x86, 0x78, 0x8f, 0x81, 0x61, 0x92, 0x8d,
  0x6d, 0x88, 0x82, 0x6c, 0x93, 0x83, 0x70, 0x89, 0x7b, 0x80, 0x85, 0x7d,
  0x79, 0x83, 0x83, 0x7c, 0x81, 0x7d, 0x80, 0x85, 0x78, 0x81, 0x86, 0x7c,
  0x7b, 0x86, 0x7f, 0x7a, 0x84, 0x80, 0x7f, 0x80, 0x79, 0x82, 0xa0, 0x56,
  0x62, 0xbe, 0x93, 0x47, 0x69, 0xb6, 0x84, 0x60, 0x7f, 0x94, 0x7a, 0x6e,
  0x8f, 0x8d, 0x6a, 0x75, 0x98, 0x83, 0x6c, 0x7e, 0x8e, 0x81, 0x74, 0x81,
  0x81, 0x81, 0x95, 0x63, 0x71, 0xa4, 0x72, 0x78, 0x91, 0x69, 0x86, 0x90,
  0x72, 0x80, 0x82, 0x7c, 0x80, 0x8a, 0x76, 0x79, 0x8d, 0x7c, 0x7c, 0x82,
  0x7d, 0x82, 0x81, 0x7b, 0x80, 0x87, 0x79, 0x7e, 0x84, 0x7f, 0x7c, 0x83,
  0x79, 0x8e, 0x8e, 0x45, 0x90, 0xbc, 0x6c, 0x46, 0x93, 0xaf, 0x61, 0x72,
  0x91, 0x80, 0x70, 0x88, 0x8f, 0x74, 0x6f, 0x8f, 0x8e, 0x6c, 0x7a, 0x90,
  0x7e, 0x74, 0x84, 0x84, 0x76, 0x8d, 0x83, 0x63, 0x91, 0x8b, 0x6f, 0x89,
  0x7f, 0x6f, 0x94, 0x7e, 0x70, 0x90, 0x78, 0x7d, 0x88, 0x7c, 0x7a, 0x85,
  0x83, 0x76, 0x84, 0x83, 0x7b, 0x83, 0x7e, 0x7c, 0x82, 0x82, 0x7e, 0x80,
  0x7e, 0x7e, 0x83, 0x7e, 0x7e, 0x80, 0x7c, 0x85, 0x7d, 0x7c, 0x9c, 0x55,
  0x6d, 0xc0, 0x85, 0x43, 0x79, 0xb7, 0x76, 0x63, 0x87, 0x90, 0x75, 0x74,
  0x94, 0x82, 0x67, 0x84, 0x95, 0x74, 0x75, 0x88, 0x87, 0x79, 0x79, 0x87,
  0x7a, 0x8a, 0x8b, 0x59, 0x8c, 0x9a, 0x68, 0x88, 0x7f, 0x70, 0x92, 0x80,
  0x74, 0x84, 0x82, 0x7a, 0x84, 0x83, 0x77, 0x85, 0x80, 0x7d, 0x84, 0x7d,
  0x7d, 0x85, 0x7f, 0x79, 0x86, 0x82, 0x7b, 0x80, 0x7f, 0x78, 0xa1, 0x6a,
  0x4f, 0xb7, 0x9f, 0x4f, 0x60, 0xb3, 0x86, 0x5e, 0x8c, 0x8c, 0x70, 0x7a,
  0x96, 0x7c, 0x6e, 0x83, 0x8f, 0x77, 0x75, 0x8b, 0x83, 0x73, 0x83, 0x88,
  0x74, 0x81, 0x96, 0x6b, 0x6d, 0xa0, 0x78, 0x73, 0x92, 0x6e, 0x7f, 0x93,
  0x6d, 0x83, 0x86, 0x77, 0x83, 0x84, 0x7c, 0x7b, 0x87, 0x7b, 0x7f, 0x85,
  0x7a, 0x7f, 0x83, 0x7f, 0x80, 0x7f, 0x7f, 0x81, 0x80, 0x7c, 0x83, 0x7f,
  0x7e, 0x7d, 0x83, 0x9b, 0x4f, 0x70, 0xc5, 0x82, 0x3f, 0x7f, 0xba, 0x6d,
  0x67, 0x8e, 0x89, 0x70, 0x7d, 0x95, 0x7c, 0x65, 0x8b, 0x98, 0x6b, 0x79,
  0x8d, 0x80, 0x78, 0x7d, 0x87, 0x78, 0x86, 0x8f, 0x5c, 0x84, 0xa0, 0x66,
  0x83, 0x88, 0x6d, 0x8e, 0x83, 0x74, 0x84, 0x82, 0x78, 0x87, 0x84, 0x72,
  0x86, 0x84, 0x77, 0x84, 0x82, 0x7c, 0x80, 0x80, 0x7e, 0x83, 0x7e, 0x7d,
  0x83, 0x7e, 0x7a, 0x85, 0x9c, 0x50, 0x6d, 0xc2, 0x84, 0x43, 0x7d, 0xbb,
  0x65, 0x69, 0x9b, 0x7e, 0x6b, 0x89, 0x94, 0x71, 0x70, 0x8e, 0x8b, 0x6e,
  0x7f, 0x8e, 0x79, 0x78, 0x87, 0x81, 0x78, 0x82, 0x91, 0x69, 0x79, 0x9c,
  0x6e, 0x7f, 0x91, 0x67, 0x8a, 0x8b, 0x6e, 0x89, 0x7d, 0x7a, 0x87, 0x83,
  0x76, 0x81, 0x87, 0x79, 0x83, 0x7d, 0x7c, 0x86, 0x7f, 0x7d, 0x7f, 0x81,
  0x80, 0x82, 0x7c, 0x80, 0x81, 0x80, 0x77, 0x96, 0x81, 0x45, 0xa0, 0xb6,
  0x5c, 0x4a, 0xaa, 0x9f, 0x58, 0x7e, 0x92, 0x79, 0x71, 0x8f, 0x8b, 0x6b,
  0x76, 0x9a, 0x7e, 0x6b, 0x88, 0x8b, 0x78, 0x77, 0x87, 0x83, 0x72, 0x95,
  0x7d, 0x5c, 0x9f, 0x86, 0x68, 0x93, 0x75, 0x77, 0x95, 0x76, 0x78, 0x88,
  0x7e, 0x78, 0x8c, 0x7c, 0x74, 0x8b, 0x7e, 0x7c, 0x84, 0x7b, 0x81, 0x83,
  0x7d, 0x7d, 0x85, 0x7d, 0x7c, 0x83, 0x81, 0x77, 0x86, 0x9a, 0x4a, 0x7b,
  0xc1, 0x75, 0x48, 0x8c, 0xb0, 0x5d, 0x78, 0x9a, 0x73, 0x6e, 0x93, 0x8b,
  0x6d, 0x79, 0x8e, 0x83, 0x6f, 0x87, 0x8a, 0x72, 0x7e, 0x89, 0x7d, 0x79,
  0x82, 0x90, 0x6a, 0x79, 0x9a, 0x71, 0x7e, 0x8d, 0x6a, 0x8c, 0x8a, 0x6d,
  0x8b, 0x7a, 0x7a, 0x8c, 0x7e, 0x77, 0x81, 0x87, 0x79, 0x83, 0x80, 0x7b,
  0x83, 0x7e, 0x80, 0x82, 0x7b, 0x7f, 0x85, 0x7d, 0x7c, 0x83, 0x7f, 0x7d,
  0x81, 0x79, 0xa0, 0x63, 0x55, 0xc1, 0x97, 0x42, 0x6e, 0xbb, 0x78, 0x5e,
  0x8f, 0x8e, 0x6c, 0x79, 0x99, 0x7d, 0x66, 0x88, 0x98, 0x6c, 0x75, 0x90,
  0x82, 0x76, 0x7c, 0x89, 0x7b, 0x7b, 0x96, 0x68, 0x74, 0xa2, 0x6d, 0x7c,
  0x92, 0x66, 0x88, 0x8f, 0x6e, 0x82, 0x85, 0x79, 0x83, 0x87, 0x73, 0x82,
  0x89, 0x77, 0x80, 0x82, 0x7b, 0x83, 0x80, 0x7c, 0x81, 0x81, 0x7b, 0x85,
  0x7f, 0x7b, 0x81, 0x84, 0x74, 0x90, 0x8d, 0x43, 0x97, 0xb8, 0x61, 0x4f,
  0xa1, 0x9e, 0x58, 0x88, 0x91, 0x6b, 0x79, 0x98, 0x7d, 0x6d, 0x85, 0x8d,
  0x79, 0x73, 0x8d, 0x84, 0x6e, 0x85, 0x89, 0x78, 0x77, 0x8e, 0x87, 0x61,
  0x8e, 0x8c, 0x6b, 0x91, 0x7a, 0x70, 0x96, 0x76, 0x7b, 0x8a, 0x78, 0x7d,
  0x88, 0x7f, 0x75, 0x8a, 0x80, 0x78, 0x81, 0x82, 0x82, 0x7d, 0x7b, 0x84,
  0x80, 0x7d, 0x80, 0x84, 0x7a, 0x81, 0x79, 0x8e, 0x94, 0x3f, 0x8a, 0xc5,
  0x67, 0x42, 0x9f, 0xa8, 0x5c, 0x77, 0x95, 0x7f, 0x68, 0x8f, 0x92, 0x69,
  0x70, 0x9c, 0x85, 0x64, 0x86, 0x8f, 0x78, 0x76, 0x85, 0x87, 0x74, 0x8a,
  0x86, 0x61, 0x94, 0x8c, 0x66, 0x90, 0x7e, 0x71, 0x92, 0x7b, 0x79, 0x87,
  0x7b, 0x7c, 0x89, 0x7e, 0x73, 0x8a, 0x80, 0x7b, 0x83, 0x7b, 0x80, 0x84,
  0x7c, 0x7f, 0x81, 0x7e, 0x7f, 0x83, 0x7d, 0x7e, 0x82, 0x7e, 0x7d, 0x7e,
  0x9e, 0x57, 0x66, 0xc3, 0x83, 0x43, 0x82, 0xb7, 0x61, 0x6f, 0x9f, 0x74,
  0x6b, 0x94, 0x8b, 0x6a, 0x7c, 0x8f, 0x81, 0x6f, 0x89, 0x89, 0x71, 0x80,
  0x8a, 0x7c, 0x77, 0x84, 0x93, 0x69, 0x74, 0x9f, 0x6f, 0x7d, 0x90, 0x68,
  0x8b, 0x89, 0x70, 0x8a, 0x7b, 0x7a, 0x89, 0x81, 0x75, 0x85, 0x86, 0x76,
  0x83, 0x81, 0x7f, 0x81, 0x7c, 0x80, 0x85, 0x79, 0x7f, 0x85, 0x7c, 0x7c,
  0x86, 0x75, 0x8e, 0x92, 0x40, 0x8e, 0xc5, 0x62, 0x43, 0xa4, 0xa7, 0x57,
  0x7b, 0x98, 0x78, 0x6a, 0x92, 0x90, 0x67, 0x75, 0x9c, 0x7f, 0x66, 0x8c,
  0x8d, 0x73, 0x78, 0x89, 0x83, 0x72, 0x91, 0x84, 0x5a, 0x99, 0x8f, 0x62,
  0x90, 0x7f, 0x71, 0x93, 0x78, 0x77, 0x8b, 0x7c, 0x79, 0x88, 0x7d, 0x79,
  0x89, 0x7c, 0x7b, 0x86, 0x7e, 0x7b, 0x83, 0x80, 0x7e, 0x80, 0x7f, 0x7f,
  0x83, 0x7b, 0x7e, 0x86, 0x7b, 0x7e
};
const uint16_t start_wav_len = 1590;
