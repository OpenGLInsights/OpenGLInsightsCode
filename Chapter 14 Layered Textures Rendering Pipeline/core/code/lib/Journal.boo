namespace kri.lib

public interface ILogger:
	def log(str as string) as void
	
public class Journal(ILogger):
	public	static	Inst		as ILogger	= null
	public	final	messages	= List[of string]()

	public def constructor():
		Inst = self
	def ILogger.log(str as string) as void:
		if str not in messages:
			messages.Add(str)
	public static def Log(str as string) as void:
		if Inst:
			Inst.log(str)
	public def flush() as string:
		if not messages.Count:
			return null
		rez = string.Join("\n",messages.ToArray())
		messages.Clear()
		return rez
	