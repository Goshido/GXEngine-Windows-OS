//version 1.0

#ifndef TS_HISTORY
#define TS_HISTORY


#include <GXEngine/GXAVLTree.h>


#define TS_HISTORY_LIMIT	6


struct TSHistoryResult
{
	GXWChar*		entries[ TS_HISTORY_LIMIT ];
	GXUInt			numEntries;
};


class TSHistoryNode;
class TSHistory : public GXAVLTree
{
	private:
		GXWChar*		historyFile;

	public:
		TSHistory ( const GXWChar* historyFile );
		~TSHistory ();

		GXVoid Find ( const GXWChar* key, TSHistoryResult &result );
		GXVoid Add ( const GXWChar* entry );
		GXVoid Print ();

	private:
		GXVoid Save ();
		GXVoid Load ();

		TSHistoryNode* FindFirst ( const GXWChar* prefix );
		GXBool IsSimular ( const GXWChar* prefix, GXWChar* entry );
		GXVoid GetTotalEntries ( TSHistoryNode* root, const GXWChar* prefix, GXUInt &numEntries, GXUInt maxtEntries );
		GXVoid GetEntries ( TSHistoryNode* root, const GXWChar** entries, const GXWChar* prefix, GXUInt &numEntries, GXUInt maxtEntries );

		GXVoid ToConsole ( TSHistoryNode* root, GXUInt level );
};


#endif //TS_HISTORY