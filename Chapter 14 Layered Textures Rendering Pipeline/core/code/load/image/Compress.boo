namespace kri.load.image

import System.IO
import OpenTK.Graphics.OpenGL
import kri.data

public class Compress( ILoaderGen[of IGenerator[of kri.buf.Texture]]):
	public static final DdsMagic		= 0x20534444
	# header flags
	[System.Flags]
	public enum HeadFlags:
		PixelFormat	= 0x00001000
		Caps		= 0x00000001
		MipCount	= 0x00020000
	# pixel format flags
	[System.Flags]
	public enum FormatFlags:
		FourCC		= 0x00000004
	# core caps
	[System.Flags]
	public enum CoreCaps:
		Complex	= 0x00000008
		Texture	= 0x00001000
		Mipmap	= 0x00400000
	# cube caps
	[System.Flags]
	public enum CubeCaps:
		Cube	= 0x00000200
		PosX	= 0x00000400
		NegX	= 0x00000800
		PosY	= 0x00001000
		NegY	= 0x00002000
		PosZ	= 0x00004000
		NegZ	= 0x00008000
		All		= PosX|NegX |PosY|NegY |PosZ|NegZ
		Volume	= 0x00200000
	# help members
	private	final	buffer	= array[of byte](16)
	private	curBlock	as uint	= 0
	private curMips		as byte = 0
	
	private struct Header:
		public magic	as ulong
		public size		as ulong
		public flags	as HeadFlags
		public height	as ulong
		public width	as ulong
		public pitch	as ulong
		public depth	as ulong
		public mips		as ulong
		public def check() as bool:
			return false	if magic != DdsMagic or size != 124
			return false	if not (flags & HeadFlags.PixelFormat) or not (flags & HeadFlags.Caps)
			return true
	
	private struct PixFormat:
		public size		as ulong
		public flags	as FormatFlags
		public fourCC	as string
		public rgbBits	as ulong
		public maskR	as ulong
		public maskG	as ulong
		public maskB	as ulong
		public maskA	as ulong
	
	private struct Caps:
		public caps1	as ulong
		public caps2	as ulong
		public ddsx		as ulong
	
	private def flipVertical(data as (byte), het as uint) as void:
		v as System.UInt64 = 0
		hasAlpha = curBlock>8
		if het<=1:	return
		if het==2:
			i=0
			while i<data.Length:
				if hasAlpha:
					v = (data[i+2]<<16) + (data[i+1]<<8) + data[i]
					data[i+0] = (v>>12)&0xFF
					data[i+1] = ((v>>20)&0xF) + ((v&0xF)<<4)
					data[i+2] = (v>>4)&0xFF
					i += 8
				x = data[i+4]
				data[i+4] = data[i+5]
				data[i+5] = x
				i += 8
			return
		totalBlocks = data.Length / curBlock
		vertiBlocks = ((het+3)>>2)
		horisBlocks = totalBlocks / vertiBlocks
		assert vertiBlocks * horisBlocks == totalBlocks
		for j in range (vertiBlocks>>1):
			for i in range(horisBlocks):
				a = curBlock * (horisBlocks*j+i)
				b = curBlock * (horisBlocks*(vertiBlocks-j-1)+i)
				System.Buffer.BlockCopy(data, a, buffer,	0, curBlock)
				System.Buffer.BlockCopy(data, b, data,		a, curBlock)
				System.Buffer.BlockCopy(buffer, 0, data,	b, curBlock)
		i=0
		while i<data.Length:
			if hasAlpha:
				v = 0
				v += (data[i+7]<<16) + (data[i+6]<<8) + data[i+5]
				v <<= 24
				v += (data[i+4]<<16) + (data[i+3]<<8) + data[i+2]
				data[i+2] = (v>>36)&0xFF
				data[i+3] = ((v>>44)&0xF) + (((v>>24)&0xF)<<4)
				data[i+4] = (v>>28)&0xFF
				data[i+5] = (v>>12)&0xFF
				data[i+6] = ((v>>20)&0xF) + (((v>>0)&0xF)<<4)
				data[i+7] = (v>>4)&0xFF
				i += 8
			x = data[i+4]
			data[i+4] = data[i+7]
			data[i+7] = x
			x = data[i+5]
			data[i+5] = data[i+6]
			data[i+6] = x
			i += 8
	
	private def readSide(br as BinaryReader, t as kri.buf.Texture, side as int) as void:
		for i in range(curMips):
			t.switchLevel(i)
			size = ((t.wid+3)>>2) * ((t.het+3)>>2) * curBlock
			data = br.ReadBytes(size)
			flipVertical(data, t.het)
			t.initCube(side,data,true)
	
	public def read(path as string) as  IGenerator[of kri.buf.Texture]:	#imp: ILoaderGen
		br = BinaryReader( File.OpenRead(path) )
		# read header
		head = Header(
			magic	: br.ReadUInt32(),
			size	: br.ReadUInt32(),
			flags	: cast( HeadFlags, br.ReadUInt32() ),
			height	: br.ReadUInt32(),
			width	: br.ReadUInt32(),
			pitch	: br.ReadUInt32(),
			depth	: br.ReadUInt32(),
			mips	: br.ReadUInt32())
		br.BaseStream.Seek(44, SeekOrigin.Current)
		# read pixel format
		format = PixFormat(
			size	: br.ReadUInt32(),
			flags	: cast( FormatFlags, br.ReadUInt32() ),
			fourCC	: string( br.ReadChars(4) ),
			rgbBits	: br.ReadUInt32(),
			maskR	: br.ReadUInt32(),
			maskG	: br.ReadUInt32(),
			maskB	: br.ReadUInt32(),
			maskA	: br.ReadUInt32())
		# read caps
		coreCaps = cast( CoreCaps, br.ReadUInt32() )
		cubeCaps = cast( CubeCaps, br.ReadUInt32() )
		br.BaseStream.Seek(12, SeekOrigin.Current)
		assert coreCaps & CoreCaps.Texture
		# check input and init texture
		if not head.check():
			kri.lib.Journal.Log("DDS: unsupported header (${path})")
			return null
		t = kri.buf.Texture()
		t.wid = head.width
		t.het = head.height
		# find internal format and block size
		curBlock = 0
		if (format.flags & FormatFlags.FourCC):
			id = System.Array.IndexOf( ('DXT1','DXT3','DXT5'), format.fourCC )
			if id<0:
				kri.lib.Journal.Log("DDS: unknown 4CC format (${format.fourCC}) of texture (${path})")
				return null
			curBlock = (8,16,16)[id]
			pif0 = (PixelInternalFormat.CompressedRgbaS3tcDxt1Ext,
					PixelInternalFormat.CompressedRgbaS3tcDxt3Ext,
					PixelInternalFormat.CompressedRgbaS3tcDxt5Ext)
			pif1 = (PixelInternalFormat.CompressedSrgbAlphaS3tcDxt1Ext,
					PixelInternalFormat.CompressedSrgbAlphaS3tcDxt3Ext,
					PixelInternalFormat.CompressedSrgbAlphaS3tcDxt5Ext)
			gc = Basic.GammaCorrected and kri.Ant.Inst.gamma
			t.intFormat = (pif0,pif1)[gc][id]
		else:
			kri.lib.Journal.Log("DDS: non-compressed (${path})")
			return null
		# check parameters
		size = ((head.width+3)>>2) * ((head.height+3)>>2) * curBlock
		if size != head.pitch:
			kri.lib.Journal.Log("DDS: invalid size (${path}: ${size} != ${head.pitch}) of texture (${path})")
			return null
		curMips = 1
		if (head.flags & HeadFlags.MipCount):
			assert coreCaps & CoreCaps.Mipmap
			curMips = head.mips
		sides = (0,)
		if (cubeCaps & CubeCaps.Cube):
			assert cubeCaps & CubeCaps.All == CubeCaps.All
			t.target = TextureTarget.TextureCubeMap
			sides = (1,-1,2,-2,3,-3)
		# read sides, finally
		for side in sides:
			readSide(br,t,side)
		# pass-through result
		t.switchLevel(0)
		t.filt(true,true)
		return Dummy( tex:t )
