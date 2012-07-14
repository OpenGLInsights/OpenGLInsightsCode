/**********************************************************************************
Stores properties of parsed Character Descriptors
*********************************************************************************/
function CharacterDescriptor(){
    /**
    this.id;
    this.x;
    this.y;
    this.width;
    this.height;  
    this.textureBuffer:  
    this.ratioFactor;
    this.aspectX;
    this.aspectY;
    */
}

/**********************************************************************************
Loads a Font Descriptor and parses the xml. 
Generates Character Descriptors.
Currently used synchronously.
*********************************************************************************/
function BitmapFontDescriptor(){
    //stores the webgl texture    
    this.texture;
    //height and width of the texture
    this.size = new Object();
	this.size.height = null;	
	this.size.width = null;
    //Hash map storing character information, Key is ASCII code
    this.chars = new Object(); 
}


BitmapFontDescriptor.prototype = {
	instantiate: function(bitmapFontId){
        var xhrArgs = {
            url: "bmfont/arialLarge.xml",  
            preventCache: true,
            handleAs: "xml",
            sync: true
        }; 
        
        var deferred = dojo.xhrGet(xhrArgs);
        var closure = this;             

        deferred.addCallback(function(xmlDoc, ioargs){
            var fontInfo = xmlDoc;
            var generalInfo = fontInfo.evaluate("//common", fontInfo, null, XPathResult.ANY_TYPE, null);
            var letters = fontInfo.evaluate("//char", fontInfo, null, XPathResult.UNORDERED_NODE_ITERATOR_TYPE, null);         
            
            generalInfo = generalInfo.iterateNext();
          	closure.size.width = parseInt(generalInfo.attributes.getNamedItem("scaleW").value);
            closure.size.height = parseInt(generalInfo.attributes.getNamedItem("scaleH").value); 
            closure.base = parseInt(generalInfo.attributes.getNamedItem("base").value);
            closure.lineHeight = parseInt(generalInfo.attributes.getNamedItem("lineHeight").value);

                        

            while(item = letters.iterateNext())
            {
                var id = item.attributes.getNamedItem("id").value;
                var x = parseInt(item.attributes.getNamedItem("x").value);
                var y = parseInt(item.attributes.getNamedItem("y").value);
                var width = parseInt(item.attributes.getNamedItem("width").value);
                var height = parseInt(item.attributes.getNamedItem("height").value);
                var xoffset = parseInt(item.attributes.getNamedItem("xoffset").value);
                var yoffset = parseInt(item.attributes.getNamedItem("yoffset").value);
                var xadvance = parseInt(item.attributes.getNamedItem("xadvance").value);
                closure.chars[id] = new CharacterDescriptor();
                closure.chars[id].id = id;
                closure.chars[id].x = x;
                closure.chars[id].y = y;
                closure.chars[id].xadvance = xadvance;
                closure.chars[id].xoffset = xoffset;
                closure.chars[id].yoffset = yoffset;
                closure.chars[id].width = width; 
                closure.chars[id].height = height;
                closure.chars[id].textureBuffer = 	[ x/closure.size.width              ,(y+height)/closure.size.height,//P1
		                                              (x+width)/closure.size.width      ,(y+height)/closure.size.height,//P2
		                                              (x+width)/closure.size.width      ,y/closure.size.height,//P3
		                                              x/closure.size.width              ,y/closure.size.height,]//P4
		    } 

        });	
	},
    getLetter: function(letter){
        return this.chars[letter.charCodeAt(0)];
    }
}
