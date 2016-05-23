// FilterText.cpp: implementation of the CFilterText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FilterText.h"

#include "Standard/RegExpM.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilterText::CFilterText()
{
	
}

CFilterText::~CFilterText()
{
	
}

CString CFilterText::enhance(CString sText)
{
	RegExpM cr;
	String sResult;
	sResult.copy( sText );

	// disable numeric html escape codes
	RegExpM::RegSearchReplace( sResult, "&(#[0-9]+)", "\\&amp;\\1" );

	// Escape every < > at first ( a bit too much, but disables all html this way )
	sResult.replace( "<", "&#60;" );
	sResult.replace( ">", "&#62;" );
	
	// Convert \n into <br>
	sResult.replace( "\n", "<br>" );
	
	// then reconvert the escaped brackets for valid tags
	// italics and bold <i> .. </i> and <b> ... </b>
	RegExpM::RegSearchReplace( sResult, "&#60;(/)?(b|B|i|I)&#62;", "<\\1\\2>" );

	// font <font color=00ff00> ... </font>
	sResult = convertPairs( sResult, "&#60;(font color=[0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f])&#62;", "<\\1>", "&#60;(/font)&#62;", "<\\1>" );
	
	// Process BBcode
	// italics and bold [i] ... [/i] and [b] ... [/b]
	RegExpM::RegSearchReplace( sResult, "\\[(/)?(b|B|i|I)\\]", "<\\1\\2>" );

	// color [color=00ff00] ... [/color]
	sResult = limitColorRange( sResult );	// limit the color first before processing the BBCode colortags
	sResult = convertPairs( sResult, "\\[(color=[0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f])\\]", "<font \\1>", "\\[/color\\]", "</font>" );

	// urls [url]ftp://ftp.palestar.com[/url] or [url=www.palestar.com]Palestar[/url] etc...
	RegExpM::RegSearchReplace( sResult, "\\[url\\](ht|f)(tp://[^ \\[\"\']+)\\[/url\\]", "<a href=\"\\1\\2\">\\1\\2</a>" );
	RegExpM::RegSearchReplace( sResult, "\\[url\\](https://[^ \\[\"\']+)\\[/url\\]", "<a href=\"\\1\">\\1</a>" );
	RegExpM::RegSearchReplace( sResult, "\\[url\\](www[^ \\[\"\']+)\\[/url\\]", "<a href=\"http://\\1\">http://\\1</a>" );
	RegExpM::RegSearchReplace( sResult, "\\[url\\](ftp[^ \\[\"\']+)\\[/url\\]", "<a href=\"ftp://\\1\">ftp://\\1</a>" );
	RegExpM::RegSearchReplace( sResult, "\\[url=(ht|f)(tp://[^ \\[\"\']+)\\]([^\\[<]+)\\[/url\\]", "<a href=\"\\1\\2\">\\3</a>" );
	RegExpM::RegSearchReplace( sResult, "\\[url=(www\\.[^ /\\[\"\'\\.]+\\.[^ <\\[\"\']+)\\]([^\\[<]+)\\[/url\\]", "<a href=\"http://\\1\">\\2</a>" );
	RegExpM::RegSearchReplace( sResult, "\\[url=(ftp\\.[^ /\\[\"\'\\.]+\\.[^ <\\[\"\']+)\\]([^\\[<]+)\\[/url\\]", "<a href=\"ftp://\\1\">\\2</a>" );
	
	
	// make standalone links clickable http://www.palestar.com
	RegExpM::RegSearchReplace( sResult, "( |>)(ht|f)(tp://[^ /\\[\"\'\\.]+\\.[^ <\\[\"\']+)", "\\1<a href=\"\\2\\3\">\\2\\3</a>" );
	RegExpM::RegSearchReplace( sResult, "( |>)(https://[^ /\\[\"\'\\.]+\\.[^ <\\[\"\']+)", "\\1<a href=\"\\2\">\\2</a>" );
	RegExpM::RegSearchReplace( sResult, "( |>)(www\\.[^ /\\[\"\'\\.]+\\.[^ <\\[\"\']+)", "\\1<a href=\"http://\\2\">http://\\2</a>" );
	RegExpM::RegSearchReplace( sResult, "( |>)(ftp\\.[^ /\\[\"\'\\.]+\\.[^ <\\[\"\']+)", "\\1<a href=\"ftp://\\2\">ftp://\\2</a>" );
	// make emails clickable
	RegExpM::RegSearchReplace( sResult, "( |>)([A-Za-z][A-Za-z0-9\\-_\\.]*)(@[A-Za-z0-9\\-]+\\.[A-Za-z0-9][A-Za-z0-9][A-Za-z0-9]?[A-Za-z0-9]?)( |<|$)", "\\1<a href=\"mailto:\\2\\3\">\\2\\3</a>\\4" );
	
	
	// place a <wbr> at least every 40 visible chars to allow a linebreak if no whitespace exists,
	// to prevent a horizontal scrollbar for long lines without whitespaces
	bool bIsVisible = true;
	int nCharsInARow = 0;
	int nEscapeChars = 0;
	int i = 0;
	while( i < sResult.length() )
	{
		char cCurrChar = sResult.buffer()[i];
		if( cCurrChar == '<' )				// Beginning of HTML-Tag -> Invisible
			bIsVisible = false;
		else if( cCurrChar == '>' )			// End of HTML-Tag -> Visible
			bIsVisible = true;
		else if( bIsVisible && cCurrChar == ' ' )	// Char is visible blankspace ? -> reset counter
			nCharsInARow = 0;
		else
			if( bIsVisible )
			{	// check if it might be an escape sequence, which may have 2 or 3 digits
				// escape sequences might actually have more than that, but are never used in this context
				if(		 ( cCurrChar == '&' ) && ( nEscapeChars == 0 ) ) nEscapeChars = 1;
				else if( ( cCurrChar == '#' ) && ( nEscapeChars == 1 ) ) nEscapeChars = 2;
				else if( ( cCurrChar >= '0' ) && ( cCurrChar <= '9' ) && ( nEscapeChars == 2 ) ) nEscapeChars = 3;
				else if( ( cCurrChar >= '0' ) && ( cCurrChar <= '9' ) && ( nEscapeChars == 3 ) ) nEscapeChars = 4;
				else if( ( cCurrChar >= '0' ) && ( cCurrChar <= '9' ) && ( nEscapeChars == 4 ) ) nEscapeChars = 5;
				else if( ( cCurrChar == ';' ) && ( nEscapeChars == 4 ) ) { nEscapeChars = 0; nCharsInARow++; i++; continue; }
				else if( ( cCurrChar == ';' ) && ( nEscapeChars == 5 ) ) { nEscapeChars = 0; nCharsInARow++; i++; continue; }
				else
				{	// not part of an escape sequence, increase the char counter and add the number of processed
					// chars that could�ve been an escape sequence but weren�t.
					nCharsInARow = nCharsInARow + 1 + nEscapeChars;
					nEscapeChars = 0;
				}
				
				// break char sequences if at limit, and no sign of an escape char, or right before the current one
				if( ( nCharsInARow >= 28 ) && ( nEscapeChars <= 1 ) )
				{	
					sResult.insert("<wbr>", i);
					nCharsInARow = 0;
					i++;
				}
			}
			
			
			i++;
	}
	
	return sResult;
}

String CFilterText::limitColorRange( String sText )
{
	RegExpM cr;
	String sTempResult = sText;
	String sResult;
	cr.RegComp( "\\[color=[0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f]\\]" );
	
	int pos = -1;
	while( ( pos = cr.RegFind( sTempResult ) ) >= 0 )	// find all colortags
	{
		// move normal text before the tag into the result
		String sTemp = sTempResult;	
		sTemp.left( pos );						
		sResult += sTemp;
		sTempResult.right( sTempResult.length() - pos );

		String colString = sTempResult;
		colString.mid( 7, 6 );									// extract colorstring

		sTempResult.remove( 0, 14 );							// remove the colortag
		
		// check color
		if( checkColor( hexToColorLong( colString ) ) != 0 )	// is outside the valid range ?
		{
			int posClose = sTempResult.findString( "[/color]" );
			if( posClose >= 0 )
				sTempResult.remove( posClose, 8 );					// remove next closing colortag
		}
		else	// color valid
			sResult += "[color=" + colString + "]";				// add accepted colortag to the result
	}
	
	sResult += sTempResult; // add remaining text

	return sResult;
}

int CFilterText::checkColor( unsigned long color )
{
	unsigned int red = color & 0xff;
	unsigned int green = ( color & 0xff00 ) >> 8;
	unsigned int blue = (color & 0xff0000) >> 16;
	
	if ( ( red + green + blue > 0x70 ) && ( red > 0x60 || green > 0x60 || blue > 0x80 ) )
		if ( red + green + blue < 624 || red < 0xb0 || green < 0xb0 || blue < 0xb0 )
			return 0;
		else
			return 1;
	else
		return -1;
}		

long CFilterText::hexToColorLong( String hex )
{
	String sRed = hex;
	sRed.left(2);
	String sGreen = hex;
	sGreen.mid(2,2);
	String sBlue = hex;
	sBlue.right(2);

	String sColor = sBlue + sGreen + sRed;

	char* pc;
	long l = strtol ( sColor, &pc, 16);
	return l;
}

// Works only for inline expressions, no anchors supported. Also < > need to be escaped before calling this.
String CFilterText::convertPairs( String text, String regExp1, String subExpr1, String regExp2, String subExpr2 )
{
	RegExpM reg1, reg2;
	reg1.RegComp( regExp1 );
	reg2.RegComp( regExp2 );

	String remaining = text;
	String result = "";
	
	int pos = -1;
	while( ( pos = reg1.RegFind( remaining ) ) >= 0 )
	{
		// move the left part of the string over to snippet and regMatch into found
		// status : remaining = "test [b]bold[/b] test"
		int snippetLen = reg1.GetFindLen();
		String found = remaining;
		found.left( pos + snippetLen );
		String snippet = found;
		found.right( snippetLen );
		snippet.left( snippet.length() - snippetLen );
		remaining.right( remaining.length() - ( pos + snippetLen ) );
		
		reg2.RegSearchReplace( snippet, "(" + regExp2 + ")", "<\\1>" );	// escape additional closing tags

		// status: snippet = "test ", found = "[b]", remaining = "bold[/b] test"

		int closePos = reg2.RegFind( remaining );
		int nextPos = reg1.RegFind( remaining );
		
		// is there a closing tag and does it appear before the next opening one ?
		if( closePos >= 0 && ( nextPos < 0  || ( closePos < nextPos ) ) )
		{
			int closeLen = reg2.GetFindLen();
			result += snippet + "{" + found + "}";
			snippet = remaining;
			snippet.left( closePos );
			String closeTag = remaining;
			closeTag.left( closePos + closeLen );
			closeTag.right( closeLen );

			result += snippet + "{" + closeTag + "}";
			remaining.right( remaining.length() - ( closePos + closeLen ) );
			// status: result = "test {[b]}bold{[/b]}", snippet = "bold", remaining = " test"
		}
		else	// illegal tag arrangement
		{
			int closeLen = reg2.GetFindLen();
			result += snippet + "<" + found + ">";
			// status: result = "test <[b]>", remaining = "bold[/b] test"
		}

	}

	reg1.RegSearchReplace( result, "{" + regExp1 + "}", subExpr1 );	// convert all good, thus nonescaped opening tags
	reg2.RegSearchReplace( result, "{" + regExp2 + "}", subExpr2 );	// convert all good, thus nonescaped closing tags

	RegExpM regFinish;
	regFinish.RegSearchReplace( result, "<(" + regExp1 + ")>", "\\1" );	// unescape the bad opening tags
	regFinish.RegSearchReplace( result, "<(" + regExp2 + ")>", "\\1" );	// unescape the bad closing tags

	reg2.RegSearchReplace( remaining, "(" + regExp2 + ")", "<\\1>" );	// escape additional closing tags

	result += remaining;	// nothing else in the remaining string needs to be converted

	return result;
}
