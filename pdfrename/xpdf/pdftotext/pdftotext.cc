//========================================================================
//
// pdftotext.cc
//
// Copyright 1997-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "GString.h"
#include "GlobalParams.h"
#include "PDFDoc.h"
#include "TextOutputDev.h"

static void appendToGooString(void *stream, const char *text, int len)
{
    ((GString *)stream)->append(text, len);
}

char *pdftotext(const char *enc, const char *eol, const char *in)
{
	globalParams = new GlobalParams(nullptr);
	globalParams->setTextPageBreaks(gFalse);
	globalParams->setTextEncoding(enc);

	GString geol(eol);
	globalParams->setTextEOL(geol.getCString());
	
	GString *inFile = new GString(in);
	PDFDoc doc(inFile, nullptr, nullptr);
	if (!doc.isOk() || !doc.okToCopy())
	{
  		delete globalParams;
		return nullptr;
	}

	GString out;
	TextOutputControl textOutControl;
	textOutControl.mode = textOutReadingOrder;

	TextOutputDev textOut(&appendToGooString, &out, &textOutControl);
	if (!textOut.isOk())
	{
  		delete globalParams;
		return nullptr;
	}
	
	doc.displayPages(&textOut, 1, doc.getNumPages(), 72, 72, 0, gFalse, gTrue, gFalse);
	char *buffer = (char*)malloc(out.getLength()+1);
	strcpy(buffer, out.getCString());

	delete globalParams;
	return buffer;
}
/*
int main(int argc, char *argv[])
{
	char *buffer = pdftotext("UTF-8", "unix", argv[1]);
	printf("%s\n", buffer);
	free(buffer);
}
*/
