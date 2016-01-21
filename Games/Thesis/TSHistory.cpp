//version 1.1

#include "TSHistory.h"
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>


struct TSHistoryFileInfo
{
	GXUInt		numEntries;
	GXUInt		entriesSize;

	GXChar*		data;

	GXUInt		headerOffset;
	GXUInt		entryOffset;
};

//-----------------------------------------------------------------------------------------------------

class TSHistoryNode : public GXAVLTreeNode
{
	public:
		GXWChar*	key;

	public:
		TSHistoryNode ( const GXWChar* entry );
		virtual ~TSHistoryNode ();
	
		virtual const GXVoid* GetKey ();

		static GXInt GXCALL Compare ( const GXVoid* a, const GXVoid* b );
};

TSHistoryNode::TSHistoryNode ( const GXWChar* entry )
{
	if ( !entry )
	{
		key = 0;
		return;
	}

	GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( entry ) + 1 );
	key = (GXWChar*)malloc ( size );
	memcpy ( key, entry, size );
}

TSHistoryNode::~TSHistoryNode ()
{
	GXSafeFree ( key );
}

const GXVoid* TSHistoryNode::GetKey ()
{
	return key;
}

GXInt GXCALL TSHistoryNode::Compare ( const GXVoid* a, const GXVoid* b )
{
	return GXWcscmp ( (const GXWChar*)a, (const GXWChar*)b );
}

//--------------------------------------------------------------------------------------------------

GXVoid GXCALL TSHistoryIteratorPresave ( const GXAVLTreeNode* node, GXVoid* args )
{
	const TSHistoryNode* n = (const TSHistoryNode*)node;
	TSHistoryFileInfo* info = (TSHistoryFileInfo*)args;

	info->numEntries++;
	info->entriesSize += sizeof ( GXWChar ) * ( GXWcslen ( n->key ) + 1 );
}

GXVoid GXCALL TSHistoryIteratorSave ( const GXAVLTreeNode* node, GXVoid* args )
{
	const TSHistoryNode* n = (const TSHistoryNode*)node;
	TSHistoryFileInfo* info = (TSHistoryFileInfo*)args;
	
	GXUInt* header = (GXUInt*)( info->data + info->headerOffset );
	*header = info->entryOffset;
	info->headerOffset += sizeof ( GXUInt );

	GXWChar* content = (GXWChar*)( info->data + info->entryOffset );
	GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( n->key ) + 1 );
	memcpy ( content, n->key, size );
	info->entryOffset += size;
}

//----------------------------------------------------------------------------------------------

TSHistory::TSHistory ( const GXWChar* historyFile ) :
GXAVLTree ( &TSHistoryNode::Compare )
{
	GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( historyFile ) + 1 );
	this->historyFile = (GXWChar*)malloc ( size );
	memcpy ( this->historyFile, historyFile, size );

	Load ();
}

TSHistory::~TSHistory ()
{
	Save ();
	free ( historyFile );
}

GXVoid TSHistory::Find ( const GXWChar* prefix, TSHistoryResult &result )
{
	TSHistoryNode* first = FindFirst ( prefix );

	if ( !first )
	{
		result.numEntries = 0;
		return;
	}
	
	GXUInt counter = 0;
	GetTotalEntries ( first, prefix, counter, TS_HISTORY_LIMIT );

	counter = 0;
	GetEntries ( first, (const GXWChar**)result.entries, prefix, counter, TS_HISTORY_LIMIT );

	result.numEntries = counter;
}

GXVoid TSHistory::Add ( const GXWChar* entry )
{
	GXAVLTree::Add ( new TSHistoryNode ( entry ) );
}

GXVoid TSHistory::Print ()
{
	ToConsole ( (TSHistoryNode*)root, 0 );
}

GXVoid TSHistory::Save ()
{
	TSHistoryFileInfo info;
	memset ( &info, 0, sizeof ( TSHistoryFileInfo ) );

	DoInfix ( root, &TSHistoryIteratorPresave, &info );

	if ( info.numEntries == 0 )
		return;

	GXUInt size = sizeof ( GXUInt ) + info.numEntries * sizeof ( GXUInt ) + info.entriesSize;
	GXChar* data = (GXChar*)malloc ( size );

	info.data = data;

	GXUInt* numEntries = (GXUInt*)data;
	*numEntries = info.numEntries;
	info.headerOffset = sizeof ( GXUInt );
	info.entryOffset = size - info.entriesSize;

	DoInfix ( root, &TSHistoryIteratorSave, &info );

	GXWriteToFile ( historyFile, (const GXVoid*)data, size );
	free ( data );
}

GXVoid TSHistory::Load ()
{
	GXChar* data;
	GXUInt size;

	GXLoadFile ( historyFile, (GXVoid**)&data, size, GX_FALSE );

	if ( !data ) return;

	GXUInt* numEntries = (GXUInt*)data;

	for ( GXUInt i = 0; i < *numEntries; i++ )
	{
		GXUInt* offset = (GXUInt*)( data + sizeof ( GXUInt ) + i * sizeof ( GXUInt ) );
		const GXWChar* entry = (const GXWChar*)( data + *offset );
		Add ( entry );
	}

	free ( data );
}

TSHistoryNode* TSHistory::FindFirst ( const GXWChar* prefix )
{
	TSHistoryNode* p = (TSHistoryNode*)root;

	GXUInt prefixSymbols = GXWcslen ( prefix );

	while ( p )
	{
		GXUInt symbols = GXWcslen ( p->key );
		GXChar compareResult;

		if ( symbols > prefixSymbols )
		{
			GXWChar old = p->key[ prefixSymbols ];
			p->key[ prefixSymbols ] = 0;

			compareResult = GXWcscmp ( prefix, p->key );
			p->key[ prefixSymbols ] = old;

			if ( compareResult == 0 )
				return p;			
		}
		else
		{
			compareResult = GXWcscmp ( prefix, p->key );
		}

		if ( compareResult < 0 )
			p = (TSHistoryNode*)p->left;
		else
			p = (TSHistoryNode*)p->right;
	}

	return 0;
}

GXBool TSHistory::IsSimular ( const GXWChar* prefix, GXWChar* entry )
{
	GXUInt prefixSymbols = GXWcslen ( prefix );
	GXUInt symbols = GXWcslen ( entry );

	if ( prefixSymbols > symbols ) return GX_FALSE;

	if ( symbols > prefixSymbols )
	{
		GXWChar old = entry[ prefixSymbols ];
		entry[ prefixSymbols ] = 0;

		GXChar compareResult = GXWcscmp ( prefix, entry );
		entry[ prefixSymbols ] = old;

		if ( compareResult == 0 ) return GX_TRUE;

		return GX_FALSE;
	}

	if ( GXWcscmp ( prefix, entry ) == 0 ) return GX_TRUE;

	return GX_FALSE;
}

GXVoid TSHistory::GetTotalEntries ( TSHistoryNode* root, const GXWChar* prefix, GXUInt &numEntries, GXUInt maxtEntries )
{
	if ( !root || numEntries >= maxtEntries ) return;

	if ( IsSimular ( prefix, root->key ) ) 
		numEntries++;

	GetTotalEntries ( (TSHistoryNode*)root->left, prefix, numEntries, maxtEntries );
	GetTotalEntries ( (TSHistoryNode*)root->right, prefix, numEntries, maxtEntries );
}

GXVoid TSHistory::GetEntries ( TSHistoryNode* root, const GXWChar** entries, const GXWChar* prefix, GXUInt &numEntries, GXUInt maxtEntries )
{
	if ( !root || numEntries >= maxtEntries ) return;

	if ( IsSimular ( prefix, root->key ) ) 
	{
		entries[ numEntries ] = root->key;
		numEntries++;
	}

	GetEntries ( (TSHistoryNode*)root->left, entries, prefix, numEntries, maxtEntries );
	GetEntries ( (TSHistoryNode*)root->right, entries, prefix, numEntries, maxtEntries );
}

GXVoid TSHistory::ToConsole ( TSHistoryNode* root, GXUInt level )
{
	if ( root )
	{
		ToConsole ( (TSHistoryNode*)root->left, level + 1 );	

		for ( GXUInt i = 0; i < level; i++ )
			GXLogW ( L"\t" );
		GXLogW ( L"%s\n", root->key );

		ToConsole ( (TSHistoryNode*)root->right, level + 1 );
	}
}