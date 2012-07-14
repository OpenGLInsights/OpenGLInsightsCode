namespace kri.load.image

import System.IO
import kri.data

public class Targa( ILoaderGen[of IGenerator[of kri.buf.Texture]] ):
	private struct Header:
		public idSize	as byte
		public cmType	as byte
		public imType	as byte
		public cmStart	as ushort
		public cmLen	as ushort
		public cmBits	as byte
		public xrig		as ushort
		public yrig		as ushort
		public wid		as ushort
		public het		as ushort
		public bits		as byte
		public descr	as byte
		public def check() as bool:
			return false	if cmType!=0 or imType != 2
			return false	if xrig + yrig
			return false	if descr<bits and bits < 24+descr
			return true

	public def read(path as string) as IGenerator[of kri.buf.Texture]:	#imp: ILoaderGen
		br = BinaryReader( File.OpenRead(path) )	
		hd = Header(
			idSize	: br.ReadByte(),
			cmType	: br.ReadByte(),	# cm = color map
			imType	: br.ReadByte(),
			cmStart	: br.ReadUInt16(),
			cmLen	: br.ReadUInt16(),
			cmBits	: br.ReadByte(),
			xrig	: br.ReadUInt16(),
			yrig	: br.ReadUInt16(),
			wid		: br.ReadUInt16(),
			het		: br.ReadUInt16(),
			bits	: br.ReadByte(),
			descr	: br.ReadByte() )
		if not hd.check():
			kri.lib.Journal.Log("Targa: bad format (${path}), image type (${hd.imType})")
			return null
		br.ReadBytes( hd.idSize )	# skip ID
		d = hd.bits>>3
		data = br.ReadBytes( hd.wid * hd.het * d )
		return Basic( path, hd.wid, hd.het, data, d )
