//version 1.0

#ifndef GX_UNREAL_STRUCTS
#define GX_UNREAL_STRUCTS


#include <GXCommon/GXMath.h>


union VJointPos
{
	struct
	{
		GXQuat	Orientation;
		GXVec3	Position;
		GXFloat	Length;       // Для теста столкновений, дебага ( не используется )
		GXVec3	XYZSize;
	};
	GXFloat v[ 11 ];
};

union VChunkHeader //Формат любого заголовка в PSK-PSA файле
{
	struct
	{
		GXChar				ChunkID [ 20 ];		// ID строка нультерминированная строка
		GXUInt				TypeFlag;			// Флаги - зарезервировано
   		GXUInt				DataSize;			// Размер на структуру
		GXUInt				DataCount;			// Количество стркуктур
	};
	GXUChar v[ 20 * sizeof ( GXChar ) + 3 * sizeof ( GXUInt ) ];
};

struct VPoint	//Элемент массива Points Data. Идет за заголовком Points Header, в котором интересно поле Datacount - количество вершин. 
{
	GXVec3	Point;
};

union VVertex	//Элемент массива Wedges Data, идущим за Wedge Header.
{
	struct
	{
		GXUShort		PointIndex;		// Индекс из массива Points Data
		GXVec2			uv;
		GXChar			MatIndex;		// At runtime, this one will be implied by the face that's pointing to us.
		GXChar			Reserved;		// Засекречено
	};
	GXUChar v[ sizeof ( GXUShort ) + sizeof ( GXVec2 ) + 2 * sizeof ( GXChar ) ];
};

union VTriangle	//Элемент массива Faces Data. Идёт сразу за заголовком Faces Header.
{
	struct
	{
		GXUShort	WedgeIndex [ 3 ];	// Указатель на три вершины с списке вершин.
		GXChar		MatIndex;			// Материалы могут быть чем угодно
		GXChar		AuxMatIndex;		// Второй материал ( не используется )
		GXULong		SmoothingGroups;	// 32-битный флаг группы сглаживания
	};
	GXUChar v[ 3 * sizeof ( GXUShort ) + 2 * sizeof ( GXChar ) + sizeof ( GXULong ) ];
};

union VMaterial //Элемент массива Materials Data. Идёт сразу за заголовком Materials Header.
{
	struct
	{
		GXChar		MaterialName [ 64 ];
		GXUInt		TextureIndex;			// Texture index ('multiskin index')
		GXULong		PolyFlags;				// Все poly's с данным материалом будут иметь это флаг.
		GXUInt		AuxMaterial;			// Зарезервировано
		GXULong		AuxFlags;				// Зарезервировано
		GXUInt		LodBias;				// Не используется
		GXUInt		LodStyle;				// Не используется
	};
	GXUChar v[ 64 * sizeof ( GXChar ) + 4 * sizeof ( GXUInt ) + 2 * sizeof ( GXULong ) ];
};

union VBone	//Элемент массива Bones Data. Идёт сразу за заголовком Bones Header.
{
	struct
	{
		GXChar		Name [ 64 ];
		GXULong	    Flags;			// Зарезервировано
		GXUInt		NumChildren;	// Не используется
		GXUInt		ParentIndex;	// 0/NULL в случае корневой кости
		VJointPos	BonePos;		// Ссылка на положение в 3D пространстве
	};
	GXUChar v[ 64 * sizeof ( GXChar ) + sizeof ( GXULong ) + 2 * sizeof ( GXUInt ) + sizeof ( VJointPos ) ];
};

union VRawBoneInfluence // Элемент массива Influences Data. Идёт сразу за заголовком Influences Header.
{
	struct
	{
		GXFloat		Weight;
		GXUInt		PointIndex;
		GXUInt		BoneIndex;
	};
	GXUChar v[ sizeof ( GXFloat ) + 2 * sizeof ( GXUInt ) ];
};

union FNamedBoneBinary	//Элемент Bones Data PSA-файла
{
	struct
	{
		GXChar			Name [ 64 ];	// Имя кости
		GXULong			Flags;			// Зарезервировано
		GXInt			NumChildren;
		GXInt			ParentIndex;	// 0 если кость корневая 
		VJointPos		BonePos;
	};
	GXUChar v[ 64 * sizeof ( GXChar ) + sizeof ( GXULong ) + 2 * sizeof ( GXInt ) + sizeof ( VJointPos ) ];
};

union AnimInfoBinary //Элемент Animations Data
{
	struct
	{
		GXChar		Name [ 64 ];			// Имя анимации
		GXChar		Group [ 64 ];			// Имя группы анимации	
		GXInt		TotalBones;				// TotalBones * NumRawFrames является количеством анимационных ключей для статистики
		GXInt		RootInclude;			// 0 нету 1 входит ( не используется )
		GXInt		KeyCompressionStyle;	// Зарезервировано
		GXInt		KeyQuotum;				// Максимальная квота для сжатия ключа	
		GXFloat		KeyReduction;			// Желательное
		GXFloat		TrackTime;				// Точное - может быть перегружено скоростью анимации
		GXFloat		AnimRate;				// FPS.
		GXInt		StartBone;				// Зарезервировано - не используется
		GXInt		FirstRawFrame;
		GXInt		NumRawFrames;			// NumRawFrames и AnimRate установленные на раскадровке
	};
	GXUChar v[ 2 * 64 * sizeof ( GXChar ) + 7 * sizeof ( GXInt ) + 3 * sizeof ( GXFloat ) ];
};

union VQuatAnimKey	//Элемент Raw keys Data
{
	struct
	{
		GXVec3	Position;           // Относительно родителя
		GXQuat	Orientation;        // Относительно родителя
		GXFloat	Time;				// Продолжительность до следующего кадра ( последний ключ прыгает на первый...)
	};
	GXFloat v[ 3 + 4 + 1 ];
};


#endif //GX_UNREAL_STRUCTS