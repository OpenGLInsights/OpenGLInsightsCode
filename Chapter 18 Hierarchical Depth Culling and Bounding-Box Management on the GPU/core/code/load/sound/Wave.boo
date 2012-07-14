namespace kri.load.sound

import System.IO
import kri.data

public class Wave( ILoaderGen[of IGenerator[of kri.sound.Buffer]] ):
	private struct Header:
		public audioFormat	as ushort
		public numChannels	as ushort
		public sampleRate	as ulong
		public byteRate		as ulong
		public blockAlign	as ushort
		public sampleBits	as ushort
		public def check() as bool:
			return numChannels * sampleBits * sampleRate == 8 * byteRate

	public def read(path as string) as IGenerator[of kri.sound.Buffer]:	#imp: ILoaderGen
		using br = BinaryReader( File.OpenRead(path) ):
			signature = string(br.ReadChars(4))
			assert signature == 'RIFF'
			chunkSize = br.ReadInt32()
			format = string(br.ReadChars(4))
			assert format == 'WAVE'
			
			signature = string(br.ReadChars(4))
			assert signature == 'fmt '
			chunkSize = br.ReadInt32()
			# read header
			hd = Header(
				audioFormat	: br.ReadInt16(),
				numChannels	: br.ReadInt16(),
				sampleRate	: br.ReadInt32(),
				byteRate	: br.ReadInt32(),
				blockAlign	: br.ReadInt16(),
				sampleBits	: br.ReadInt16() )
			assert hd.check()
			
			signature = string(br.ReadChars(4))
			assert signature == 'data'
			chunkSize = br.ReadInt32()
			data = br.ReadBytes(chunkSize)
		return Basic(hd.numChannels, hd.sampleBits, hd.sampleRate, data)
