/**
 * SyntaxHighlighter
 * http://alexgorbatchev.com/SyntaxHighlighter
 *
 * SyntaxHighlighter is donationware. If you are using it, please donate.
 * http://alexgorbatchev.com/SyntaxHighlighter/donate.html
 *
 * @version
 * 3.0.83 (July 02 2010)
 * 
 * @copyright
 * Copyright (C) 2004-2010 Alex Gorbatchev. This GLSL brush by Stefan Gustavson.
 *
 * @license
 * Dual licensed under the MIT and GPL licenses.
 */
;(function()
{
	// CommonJS
	typeof(require) != 'undefined' ? SyntaxHighlighter = require('shCore').SyntaxHighlighter : null;

	function Brush()
	{
	// GLSL 1.20, works also for GLSL for OGL ES 2.0 and WebGL 1.0
		var datatypes =	'attribute const uniform varying centroid invariant ' +
						'struct in out inout float int void bool ' +
						'mat2 mat3 mat4 mat2x2 mat2x3 mat2x4 ' +
						'mat3x2 mat3x3 mat3x4 mat4x2 mat4x3 mat4x4 ' +
						'vec2 vec3 vec4 ivec2 ivec3 ivec4 bvec2 bvec3 bvec4 ' +
						'sampler1D sampler2D sampler3D samplerCube ' +
						'sampler1DShadow sampler2DShadow';

		var keywords =	'true false break continue do for while if else discard return';
					
		var functions =	'dFdx dFdy fwidth noise1 noise2 noise3 noise4 ' +
						'radians degrees sin cos tan asin acos atan ' +
						'pow exp log exp2 log2 sqrt inversesqrt ' +
						'abs sign floor ceil fract mod min max clamp mix step smoothstep ' +
						'length distance dot cross normalize ftransform ' +
						'faceforward reflect refract ' +
						'matrixCompMult outerProduct transpose ' +
						'lessThan lessThanEqual greaterThan greaterThanEqual ' +
						'equal notEqual any all not ' +
						'texture1D texture1DProj texture1DLod texture1DProjLod ' +
						'texture2D texture2DProj texture2DLod texture2DProjLod ' +
						'texture3D texture3DProj texture3DLod texture3DProjLod ' +
						'textureCube textureCubeLod' +
						'shadow1D shadow2D shadow1DProj shadow2DProj ' +
						'shadow1DLod shadow2DLod shadow1DProjLod shadow2DProjLod' +
						'dFdx dFdy fwidth noise1 noise2 noise3 noise4';

		this.regexList = [
			{ regex: SyntaxHighlighter.regexLib.singleLineCComments,	css: 'comments' },			// one line comments
			{ regex: SyntaxHighlighter.regexLib.multiLineCComments,		css: 'comments' },			// multiline comments
			{ regex: /^ *#.*/gm,										css: 'preprocessor' },
			{ regex: new RegExp(this.getKeywords(datatypes), 'gm'),		css: 'color1 bold' },
			{ regex: new RegExp(this.getKeywords(functions), 'gm'),		css: 'functions bold' },
			{ regex: new RegExp(this.getKeywords(keywords), 'gm'),		css: 'keyword bold' }
			];

		this.forHtmlScript({
			left	: /(&lt;|<)%[@!=]?/g, 
			right	: /%(&gt;|>)/g 
		});
	};

	Brush.prototype	= new SyntaxHighlighter.Highlighter();
	Brush.aliases	= ['GLSL'];

	SyntaxHighlighter.brushes.GLSL = Brush;

	// CommonJS
	typeof(exports) != 'undefined' ? exports.Brush = Brush : null;
})();
