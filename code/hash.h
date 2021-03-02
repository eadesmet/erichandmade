#ifndef HASH_H
#define HASH_H


// TODO(Eric): NOT USED, but should make one in the future
struct hash_table
{
	s32 Key;
	u8 Value;
	hash_table *Next;
};



// NOTE(Eric): Because I don't know where else to put it. I'm tired.
struct ast_slot
{
	v2 Index;
	bool32 Taken;
};

#endif //HASH_H
