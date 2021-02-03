#pragma once 
#define SAFE_RELEASE(x) if( x ) { (x)->Release(); (x) = NULL; }
#define SAFE_DELETE(x) if( x ) { delete(x); (x) = NULL; }
#define SAFE_DELETE_ARRAY(x) if( x ) { delete [] (x); (x) = NULL; }
#define SHOWERROR(s,f,l) char buf[1024]; sprintf( buf, "File: %s\nLine: %d\n%s",f,l,s); MessageBox( 0, buf, "Error", 0 );
#define SHOWERROR_W(s,f,l) wchar_t buf[1024]; wsprintf(buf, L"File: %s\nLine: %d\n%s",f,l,s); MessageBox(0, buf, L"Error", 0);

#define nameof(symbol) #symbol
