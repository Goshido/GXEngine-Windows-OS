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
		GXFloat	Length;       // ��� ����� ������������, ������ ( �� ������������ )
		GXVec3	XYZSize;
	};
	GXFloat v[ 11 ];
};

union VChunkHeader //������ ������ ��������� � PSK-PSA �����
{
	struct
	{
		GXChar				ChunkID [ 20 ];		// ID ������ ������������������� ������
		GXUInt				TypeFlag;			// ����� - ���������������
   		GXUInt				DataSize;			// ������ �� ���������
		GXUInt				DataCount;			// ���������� ���������
	};
	GXUChar v[ 20 * sizeof ( GXChar ) + 3 * sizeof ( GXUInt ) ];
};

struct VPoint	//������� ������� Points Data. ���� �� ���������� Points Header, � ������� ��������� ���� Datacount - ���������� ������. 
{
	GXVec3	Point;
};

union VVertex	//������� ������� Wedges Data, ������ �� Wedge Header.
{
	struct
	{
		GXUShort		PointIndex;		// ������ �� ������� Points Data
		GXVec2			uv;
		GXChar			MatIndex;		// At runtime, this one will be implied by the face that's pointing to us.
		GXChar			Reserved;		// �����������
	};
	GXUChar v[ sizeof ( GXUShort ) + sizeof ( GXVec2 ) + 2 * sizeof ( GXChar ) ];
};

union VTriangle	//������� ������� Faces Data. ��� ����� �� ���������� Faces Header.
{
	struct
	{
		GXUShort	WedgeIndex [ 3 ];	// ��������� �� ��� ������� � ������ ������.
		GXChar		MatIndex;			// ��������� ����� ���� ��� ������
		GXChar		AuxMatIndex;		// ������ �������� ( �� ������������ )
		GXULong		SmoothingGroups;	// 32-������ ���� ������ �����������
	};
	GXUChar v[ 3 * sizeof ( GXUShort ) + 2 * sizeof ( GXChar ) + sizeof ( GXULong ) ];
};

union VMaterial //������� ������� Materials Data. ��� ����� �� ���������� Materials Header.
{
	struct
	{
		GXChar		MaterialName [ 64 ];
		GXUInt		TextureIndex;			// Texture index ('multiskin index')
		GXULong		PolyFlags;				// ��� poly's � ������ ���������� ����� ����� ��� ����.
		GXUInt		AuxMaterial;			// ���������������
		GXULong		AuxFlags;				// ���������������
		GXUInt		LodBias;				// �� ������������
		GXUInt		LodStyle;				// �� ������������
	};
	GXUChar v[ 64 * sizeof ( GXChar ) + 4 * sizeof ( GXUInt ) + 2 * sizeof ( GXULong ) ];
};

union VBone	//������� ������� Bones Data. ��� ����� �� ���������� Bones Header.
{
	struct
	{
		GXChar		Name [ 64 ];
		GXULong	    Flags;			// ���������������
		GXUInt		NumChildren;	// �� ������������
		GXUInt		ParentIndex;	// 0/NULL � ������ �������� �����
		VJointPos	BonePos;		// ������ �� ��������� � 3D ������������
	};
	GXUChar v[ 64 * sizeof ( GXChar ) + sizeof ( GXULong ) + 2 * sizeof ( GXUInt ) + sizeof ( VJointPos ) ];
};

union VRawBoneInfluence // ������� ������� Influences Data. ��� ����� �� ���������� Influences Header.
{
	struct
	{
		GXFloat		Weight;
		GXUInt		PointIndex;
		GXUInt		BoneIndex;
	};
	GXUChar v[ sizeof ( GXFloat ) + 2 * sizeof ( GXUInt ) ];
};

union FNamedBoneBinary	//������� Bones Data PSA-�����
{
	struct
	{
		GXChar			Name [ 64 ];	// ��� �����
		GXULong			Flags;			// ���������������
		GXInt			NumChildren;
		GXInt			ParentIndex;	// 0 ���� ����� �������� 
		VJointPos		BonePos;
	};
	GXUChar v[ 64 * sizeof ( GXChar ) + sizeof ( GXULong ) + 2 * sizeof ( GXInt ) + sizeof ( VJointPos ) ];
};

union AnimInfoBinary //������� Animations Data
{
	struct
	{
		GXChar		Name [ 64 ];			// ��� ��������
		GXChar		Group [ 64 ];			// ��� ������ ��������	
		GXInt		TotalBones;				// TotalBones * NumRawFrames �������� ����������� ������������ ������ ��� ����������
		GXInt		RootInclude;			// 0 ���� 1 ������ ( �� ������������ )
		GXInt		KeyCompressionStyle;	// ���������������
		GXInt		KeyQuotum;				// ������������ ����� ��� ������ �����	
		GXFloat		KeyReduction;			// �����������
		GXFloat		TrackTime;				// ������ - ����� ���� ����������� ��������� ��������
		GXFloat		AnimRate;				// FPS.
		GXInt		StartBone;				// ��������������� - �� ������������
		GXInt		FirstRawFrame;
		GXInt		NumRawFrames;			// NumRawFrames � AnimRate ������������� �� �����������
	};
	GXUChar v[ 2 * 64 * sizeof ( GXChar ) + 7 * sizeof ( GXInt ) + 3 * sizeof ( GXFloat ) ];
};

union VQuatAnimKey	//������� Raw keys Data
{
	struct
	{
		GXVec3	Position;           // ������������ ��������
		GXQuat	Orientation;        // ������������ ��������
		GXFloat	Time;				// ����������������� �� ���������� ����� ( ��������� ���� ������� �� ������...)
	};
	GXFloat v[ 3 + 4 + 1 ];
};


#endif //GX_UNREAL_STRUCTS