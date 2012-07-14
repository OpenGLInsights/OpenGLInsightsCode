namespace viewer

import System.Net


public class ExceptApp:
	[Glade.Widget]	exceptionDialog		as Gtk.Dialog
	[Glade.Widget]	textMessage			as Gtk.Label
	[Glade.Widget]	butReport			as Gtk.Button
	[Glade.Widget]	entryName			as Gtk.Entry
	
	private	final	cryptor	= System.Security.Cryptography.MD5.Create()
	private	final	ftpHost = 'ftp://kvatom.com/kri/'
	private	final	auth	= NetworkCredential('god@kvatom.com','god')
	private final	doResp	as int
	private	final	dialog	as Gtk.MessageDialog
	private			subEx	as System.Exception = null
	public			sceneFile	= ''
	
	public def constructor():
		xml = Glade.XML('scheme/exception.glade', 'exceptionDialog', null)
		xml.Autoconnect(self)
		doResp = exceptionDialog.GetResponseForWidget(butReport)
		dialog = Gtk.MessageDialog(exceptionDialog, Gtk.DialogFlags.Modal,
			Gtk.MessageType.Info, Gtk.ButtonsType.Ok, null)
		dialog.Title = 'Response from FTP server:'
		textMessage.Text = null
	
	private def encode(bar as (byte)) as string:
		return string.Join('',
			List[of string](b.ToString('X2') for b in bar).
			ToArray())
	
	private def getSystemInfo() as string:
		os		= System.Environment.OSVersion.VersionString
		net		= System.Environment.Version.ToString()
		host	= System.Environment.MachineName
		user	= "${System.Environment.UserName}@${System.Environment.UserDomainName}"
		return "\nOS: ${os}\nPlatform: ${net}\nHost: ${host}\nUser: ${user}"
	
	private def makeRequest(uri as string, m as string) as FtpWebRequest:
		r = FileWebRequest.Create(uri) as FtpWebRequest
		r.UseBinary = r.KeepAlive = true
		r.Credentials = auth
		r.Method = m
		return r
	
	private def uploadData(uri as string, data as (byte)) as FtpWebResponse:
		req = makeRequest(uri, WebRequestMethods.Ftp.UploadFile)
		subEx = null
		try:
			stream = req.GetRequestStream()
			stream.Write( data, 0, data.Length )
			stream.Close()
			response = req.GetResponse() as FtpWebResponse
		except e as System.Exception:
			subEx = e
		return response
	
	private def uploadString(uri as string, data as string) as FtpWebResponse:
		bs = System.Text.Encoding.UTF8.GetBytes(data)
		return uploadData(uri,bs)
	
	private def uploadFile(uri as string, path as string) as FtpWebResponse:
		bs = System.IO.File.ReadAllBytes(path)
		return uploadData(uri,bs)
	
	
	public def show(str as string) as bool:
		textMessage.Text = str
		System.IO.File.WriteAllText( 'exception.txt', str )
		stage = ''
		while exceptionDialog.Run() == doResp:
			response as FtpWebResponse = null
			subEx = null
			bs = System.Text.Encoding.UTF8.GetBytes(str)
			hash = cryptor.ComputeHash(bs)
			path = ftpHost + encode(hash)
			stage = 'MakeDir'
			req = makeRequest( path, WebRequestMethods.Ftp.MakeDirectory )
			try:
				response = req.GetResponse() as FtpWebResponse
			except e as System.Exception:
				#if not response:	subEx = e
				break
			stage = 'UploadMessage'
			response = uploadString( path+'/ex.txt', str )
			if subEx: break
			sysInfo = getSystemInfo()
			glInfo = alInfo = ''
			if kri.Ant.Inst:
				glInfo = kri.Ant.Inst.caps.getInfo()
			if kri.sound.Opera.Inst:
				alInfo = kri.sound.Opera.Inst.caps.getInfo()
			info = "Signature: ${entryName.Text}\n${sysInfo}\n${glInfo}\n${alInfo}"
			stage = 'UploadInfo'
			response = uploadString( path+'/info.txt', info )
			if subEx: break
			if not string.IsNullOrEmpty(sceneFile):
				stage = 'UploadScene'
				response = uploadFile( path+'/test.scene', sceneFile )
				if subEx: break
			stage = 'Done'
			break
		if not string.IsNullOrEmpty(stage):
			s = ''
			if response:
				s = response.StatusCode.ToString()
			elif subEx:
				s = subEx.ToString()
			else:
				s = 'Issue is already known'
			dialog.Text = "Stage = ${stage}:\n${s}"
			dialog.Run()
			dialog.Hide()
		exceptionDialog.Hide()
		return true
