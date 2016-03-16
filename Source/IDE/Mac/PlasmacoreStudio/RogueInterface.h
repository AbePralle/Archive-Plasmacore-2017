#ifndef ROGUE_INTERFACE_H
#define ROGUE_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

void RogueInterface_set_arg_count( int count );
void RogueInterface_set_arg_value( int index, const char* value );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ROGUE_INTERFACE_H
