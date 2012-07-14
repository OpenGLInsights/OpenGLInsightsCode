namespace kri.lib

import System
import OpenTK.Graphics

#	General configuration file

public class Config:
	private final dict	= Collections.Generic.Dictionary[of string,string]()
	public def constructor():
		pass
	public def constructor(path as string):
		read(path)
	public def read(path as string) as void:
		if not IO.File.Exists(path):
			Journal.Log("Config (${path}) not found")
			return
		for line in IO.File.ReadAllLines(path):
			if line =~ /^\s*#/:	continue
			name,val = /\s*=\s*/.Split(line)
			dict[name] = val
	public def ask(name as string, default as string) as string:
		rez as string = null
		if dict.TryGetValue(name,rez):
			dict[name] = null
			return rez
		return default
	public def getUnused() as string*:
		return ( d.Key	for d in dict	if d.Value!=null )


#	GL context and framebuffer options

public struct OptionReader:
	public final	verMajor	as byte
	public final	verMinor	as byte
	public final	debug		as bool
	public final	buffers		as byte
	public final	samples		as byte
	public final	stereo		as bool
	public final	gamma		as bool
	
	public def constructor(conf as Config):
		# read GL context flags
		context	= conf.ask('GL.Context','3.2')
		debug = context.EndsWith('d')
		if debug:	context = context.TrimEnd(char('d'))
		ver = context.Split(char('.'))
		verMajor,verMinor = 3,2
		if ver.Length>1:	verMajor = byte.Parse(ver[0])
		if ver.Length>0:	verMinor = byte.Parse(ver[-1])
		# read main FB flags
		samples	= byte.Parse(conf.ask('FB.Samples','0'))
		buffers	= byte.Parse(conf.ask('FB.Buffers','2'))
		stereo	= conf.ask('FB.Stereo','no')	== 'yes'
		gamma	= conf.ask('FB.Gamma','no')	== 'yes'
	
	public def genMode(bpp as byte, depth as byte) as GraphicsMode:
		stencil = (0,8)[depth==24]
		return GraphicsMode( ColorFormat(bpp), depth, stencil,
			samples, ColorFormat.Empty, buffers, stereo )
	
	public def genFlags() as GraphicsContextFlags:
		rez  = GraphicsContextFlags.ForwardCompatible
		rez |= GraphicsContextFlags.Debug	if debug
		return rez
