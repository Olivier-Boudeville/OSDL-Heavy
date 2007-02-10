#ifndef OSDL_KEYBOARD_HANDLER_H_
#define OSDL_KEYBOARD_HANDLER_H_


#include "OSDLInputDeviceHandler.h"   // for inheritance
#include "OSDLEventsCommon.h"     // for KeyModifier

#include "Ceylan.h"               // for Unicode

#include <string>
#include <list>
#include <map>



namespace OSDL
{


	namespace MVC
	{
	
		// Keyboard handler is linked to controllers.
		class Controller ;
		
	}
	
		
	namespace Events
	{
			
		
		/**
		 * Pointer to functions managing specific keyboard events, i.e. 
		 * a specific set of keys that may be pressed or released.
		 *
		 * @see for instance doNothingKeyHandler and smarterKeyHandler in
		 * OSDLKeyboardHandler.cc.
		 * 
		 * @note The compiler refuses to force exception specification in
		 * typedef thanks to a 'throw()' : 
		 * "error: OSDL::Events::KeyboardEventHandler declared with an exception
		 * specification."
		 *
		 */ 
		typedef void (* KeyboardEventHandler)( 
			const KeyboardEvent & keyboardEvent ) /* throw() */ ;
		
		
		
		/// Mother class for all keyboard exceptions. 		
		class OSDL_DLL KeyboardException: public EventsException 
		{
			public: 
				explicit KeyboardException( const std::string & reason ) 
					throw() ; 
				virtual ~KeyboardException() throw() ; 
				
		} ;


				
		/**
		 * Describes what is the current mode used to handle key presses 
		 * and key releases. 
		 *
		 * - 'rawInput' uses basically scancodes to track only keys being hit 
		 * or realeased, as if the keyboard was a big game pad with numerous
		 * buttons
		 * - 'textInput' is used for text input with unicode support, scan 
		 * codes are converted according to keyboard layout so that actual
		 * characters are returned.
		 *
		 * @note None of these modes can be totally satisfactory, since many
		 * keyboard layouts exist and any locale can be mapped onto it. 
		 * The most reliable solution is doing like commercial games, 
		 * which uses a settings screen allowing the user to specify 
		 * key-action associations.
		 *
		 * @note Keyboard starts in raw input mode.
		 *
		 */
		enum KeyboardMode { rawInput, textInput } ;

		
		/**
		 * Handler for keyboard.
		 *
		 * Keyboard input are managed according to the current keyboard mode
		 * (example : raw input, text input with unicode support, etc.)
		 *
		 */
		class OSDL_DLL KeyboardHandler : public InputDeviceHandler
		{
		
			
			public:



				/**
				 * List of all currently available key identifiers : allows 
				 * to identify a keyboard key being pressed or released.
				 *
				 * First column lists the key identifiers which are to be
				 * used in user applications, second one maps them to the
				 * keys defined by the SDL back-end, and third describes 
				 * the corresponding key.
				 *
				 * The source of the complete list is the SDL back-end
				 * (SDL_keysym.h).
				 *
				 */			
				enum KeyIdentifier
				{
				
				
					/*
					 * The keyboard identifiers have been cleverly chosen to 
					 * map to ASCII :
					 *
					 */
				
					// For unknown keys :	
					//UnknownKey        = SDLK_UNKNOWN      ,  
					
					//FirstKey          = SDLK_FIRST        , // 
					
					BackspaceKey        = SDLK_BACKSPACE    , // 
					TabKey              = SDLK_TAB          , // 
					ClearKey            = SDLK_CLEAR        , // 
					
					// See also : EnterKeypadKey
					EnterKey            = SDLK_RETURN       , 
					
					 // Synonym of EnterKey :
					ReturnKey           = SDLK_RETURN       ,
					
					PauseKey            = SDLK_PAUSE        , // 
					EscapeKey           = SDLK_ESCAPE       , // 
					SpaceKey            = SDLK_SPACE        , // 
					ExclaimKey          = SDLK_EXCLAIM      , // 
					QuotedBLKey         = SDLK_QUOTEDBL     , // 
					HashKey             = SDLK_HASH         , // 
					DollarKey           = SDLK_DOLLAR       , // 
					AmpersandKey        = SDLK_AMPERSAND    , // 
					QuoteKey            = SDLK_QUOTE        , // 
					LeftParenthesisKey  = SDLK_LEFTPAREN    , // 
					RightParenthesisKey = SDLK_RIGHTPAREN   , // 
					AsteriskKey         = SDLK_ASTERISK     , // 
					PlusKey             = SDLK_PLUS         , // 
					CommaKey            = SDLK_COMMA        , // 
					MinusKey            = SDLK_MINUS        , // 
					PeriodKey           = SDLK_PERIOD       , // 
					SlashKey            = SDLK_SLASH        , // 
					ZeroKey             = SDLK_0            , // 
					OneKey              = SDLK_1            , // 
					TwoKey              = SDLK_2            , // 
					ThreeKey            = SDLK_3            , // 
					FourKey             = SDLK_4            , // 
					FiveKey             = SDLK_5            , // 
					SixKey              = SDLK_6            , // 
					SevenKey            = SDLK_7            , // 
					HeightKey           = SDLK_8            , // 
					NineKey             = SDLK_9            , // 
					ColonKey            = SDLK_COLON        , // 
					SemiColonKey        = SDLK_SEMICOLON    , // 
					LessKey             = SDLK_LESS         , // 
					EqualsKey           = SDLK_EQUALS       , // 
					GreaterKey          = SDLK_GREATER      , // 
					QuestionKey         = SDLK_QUESTION     , // 
					AtKey               = SDLK_AT           , // 

					
					// Skip uppercase letters.
					
					LeftBracketKey      = SDLK_LEFTBRACKET  , // 
					RightBracket        = SDLK_RIGHTBRACKET , // 
					BackslashKey        = SDLK_BACKSLASH    , // 
					Caret               = SDLK_CARET        , // 
					Underscore          = SDLK_UNDERSCORE   , // 
					BackQuote           = SDLK_BACKQUOTE    , // 
					aKey                = SDLK_a            , // 
					bKey                = SDLK_b            , // 
					cKey                = SDLK_c            , // 
					dKey                = SDLK_d            , // 
					eKey                = SDLK_e            , // 
					fKey                = SDLK_f            , // 
					gKey                = SDLK_g            , // 
					hKey                = SDLK_h            , // 
					iKey                = SDLK_i            , // 
					jKey                = SDLK_j            , // 
					kKey                = SDLK_k            , // 
					lKey                = SDLK_l            , // 
					mKey                = SDLK_m            , // 
					nKey                = SDLK_n            , // 
					oKey                = SDLK_o            , // 
					pKey                = SDLK_p            , // 
					qKey                = SDLK_q            , // 
					rKey                = SDLK_r            , // 
					sKey                = SDLK_s            , // 
					tKey                = SDLK_t            , // 
					uKey                = SDLK_u            , // 
					vKey                = SDLK_v            , // 
					wKey                = SDLK_w            , // 
					xKey                = SDLK_x            , // 
					yKey                = SDLK_y            , // 
					zKey                = SDLK_z            , // 
					DeleteKey           = SDLK_DELETE       , // 
					
					// End of ASCII mapped key identifiers.
					
					
					// International keyboard identifiers :

					International0Key = SDLK_WORLD_0        , // 0xA0.
					International1Key = SDLK_WORLD_1        ,
					International2Key = SDLK_WORLD_2        ,
					International3Key = SDLK_WORLD_3        ,
					International4Key = SDLK_WORLD_4        ,
					International5Key = SDLK_WORLD_5        ,
					International6Key = SDLK_WORLD_6        ,
					International7Key = SDLK_WORLD_7        ,
					International8Key = SDLK_WORLD_8        ,
					International9Key = SDLK_WORLD_9        ,
					
					International10Key = SDLK_WORLD_10      ,
					International11Key = SDLK_WORLD_11      ,
					International12Key = SDLK_WORLD_12      ,
					International13Key = SDLK_WORLD_13      ,
					International14Key = SDLK_WORLD_14      ,
					International15Key = SDLK_WORLD_15      ,
					International16Key = SDLK_WORLD_16      ,
					International17Key = SDLK_WORLD_17      ,
					International18Key = SDLK_WORLD_18      ,
					International19Key = SDLK_WORLD_19      ,
					
					International20Key = SDLK_WORLD_20      ,
					International21Key = SDLK_WORLD_21      ,
					International22Key = SDLK_WORLD_22      ,
					International23Key = SDLK_WORLD_23      ,
					International24Key = SDLK_WORLD_24      ,
					International25Key = SDLK_WORLD_25      ,
					International26Key = SDLK_WORLD_26      ,
					International27Key = SDLK_WORLD_27      ,
					International28Key = SDLK_WORLD_28      ,
					International29Key = SDLK_WORLD_29      ,
					
					International30Key = SDLK_WORLD_30      ,
					International31Key = SDLK_WORLD_31      ,
					International32Key = SDLK_WORLD_32      ,
					International33Key = SDLK_WORLD_33      ,
					International34Key = SDLK_WORLD_34      ,
					International35Key = SDLK_WORLD_35      ,
					International36Key = SDLK_WORLD_36      ,
					International37Key = SDLK_WORLD_37      ,
					International38Key = SDLK_WORLD_38      ,
					International39Key = SDLK_WORLD_39      ,
								    		   
					International40Key = SDLK_WORLD_40      ,
					International41Key = SDLK_WORLD_41      ,
					International42Key = SDLK_WORLD_42      ,
					International43Key = SDLK_WORLD_43      ,
					International44Key = SDLK_WORLD_44      ,
					International45Key = SDLK_WORLD_45      ,
					International46Key = SDLK_WORLD_46      ,
					International47Key = SDLK_WORLD_47      ,
					International48Key = SDLK_WORLD_48      ,
					International49Key = SDLK_WORLD_49      ,
	
					International50Key = SDLK_WORLD_50      ,
					International51Key = SDLK_WORLD_51      ,
					International52Key = SDLK_WORLD_52      ,
					International53Key = SDLK_WORLD_53      ,
					International54Key = SDLK_WORLD_54      ,
					International55Key = SDLK_WORLD_55      ,
					International56Key = SDLK_WORLD_56      ,
					International57Key = SDLK_WORLD_57      ,
					International58Key = SDLK_WORLD_58      ,
					International59Key = SDLK_WORLD_59      ,
	
					International60Key = SDLK_WORLD_60      ,
					International61Key = SDLK_WORLD_61      ,
					International62Key = SDLK_WORLD_62      ,
					International63Key = SDLK_WORLD_63      ,
					International64Key = SDLK_WORLD_64      ,
					International65Key = SDLK_WORLD_65      ,
					International66Key = SDLK_WORLD_66      ,
					International67Key = SDLK_WORLD_67      ,
					International68Key = SDLK_WORLD_68      ,
					International69Key = SDLK_WORLD_69      ,
	
					International70Key = SDLK_WORLD_70      ,
					International71Key = SDLK_WORLD_71      ,
					International72Key = SDLK_WORLD_72      ,
					International73Key = SDLK_WORLD_73      ,
					International74Key = SDLK_WORLD_74      ,
					International75Key = SDLK_WORLD_75      ,
					International76Key = SDLK_WORLD_76      ,
					International77Key = SDLK_WORLD_77      ,
					International78Key = SDLK_WORLD_78      ,
					International79Key = SDLK_WORLD_79      ,
	
					International80Key = SDLK_WORLD_80      ,
					International81Key = SDLK_WORLD_81      ,
					International82Key = SDLK_WORLD_82      ,
					International83Key = SDLK_WORLD_83      ,
					International84Key = SDLK_WORLD_84      ,
					International85Key = SDLK_WORLD_85      ,
					International86Key = SDLK_WORLD_86      ,
					International87Key = SDLK_WORLD_87      ,
					International88Key = SDLK_WORLD_88      ,
					International89Key = SDLK_WORLD_89      ,
	
					International90Key = SDLK_WORLD_90      ,
					International91Key = SDLK_WORLD_91      ,
					International92Key = SDLK_WORLD_92      ,
					International93Key = SDLK_WORLD_93      ,
					International94Key = SDLK_WORLD_94      ,
					International95Key = SDLK_WORLD_95      , // 0xFF.

					// End of international keyboard identifiers.
					
					
					// Numeric keypad :
					ZeroKeypadKey       = SDLK_KP0          , // 
					OneKeypadKey        = SDLK_KP1          , // 
					TwoKeypadKey        = SDLK_KP2          , // 
					ThreeKeypadKey      = SDLK_KP3          , // 
					FourKeypadKey       = SDLK_KP4          , // 
					FiveKeypadKey       = SDLK_KP5          , // 
					SixKeypadKey        = SDLK_KP6          , // 
					SevenKeypadKey      = SDLK_KP7          , // 
					HeightKeypadKey     = SDLK_KP8          , // 
					NineKeypadKey       = SDLK_KP9          , // 


					// Rest of keypad :
					PeriodKeypadKey     = SDLK_KP_PERIOD    , // 
					DivideKeypadKey     = SDLK_KP_DIVIDE    , // 
					MultiplyKeypadKey   = SDLK_KP_MULTIPLY  , // 
					MinusKeypadKey      = SDLK_KP_MINUS     , // 
					PlusKeypadKey       = SDLK_KP_PLUS      , //
					
					// See also : EnterKey. 
					EnterKeypadKey      = SDLK_KP_ENTER     , 
					EqualsKeypadKey     = SDLK_KP_EQUALS    , // 


					// Arrows and Home/End pad :
					UpArrowKey          = SDLK_UP           , // 
					DownArrowKey        = SDLK_DOWN         , // 
					LeftArrowKey        = SDLK_LEFT         , // 
					RightArrowKey       = SDLK_RIGHT        , // 
					InsertKey           = SDLK_INSERT       , // 
					HomeKey             = SDLK_HOME         , // 
					EndKey              = SDLK_END          , // 
					PageUpKey           = SDLK_PAGEUP       , // 
					PageDownKey         = SDLK_PAGEDOWN     , //  


					// Function keys :
 					F1Key               = SDLK_F1           , // 
 					F2Key               = SDLK_F2           , // 
 					F3Key               = SDLK_F3           , // 
 					F4Key               = SDLK_F4           , // 
 					F5Key               = SDLK_F5           , // 
 					F6Key               = SDLK_F6           , // 
 					F7Key               = SDLK_F7           , // 
 					F8Key               = SDLK_F8           , // 
 					F9Key               = SDLK_F9           , // 
 					F10Key              = SDLK_F10          , // 
 					F11Key              = SDLK_F11          , // 
 					F12Key              = SDLK_F12          , // 
 					F13Key              = SDLK_F13          , // 
 					F14Key              = SDLK_F14          , // 
 					F15Key              = SDLK_F15          , // 
					
					
					// Key state modifier keys :
					NumLockKey          = SDLK_NUMLOCK      , // 
					CapsLockKey         = SDLK_CAPSLOCK     , // 
					ScrolLockKey        = SDLK_SCROLLOCK    , // 
					LeftShiftKey        = SDLK_LSHIFT       , // 
					RightShiftKey       = SDLK_RSHIFT       , // 
					LeftControlKey      = SDLK_LCTRL        , // 
					RightControlKey     = SDLK_RCTRL        , // 
					LeftAltKey          = SDLK_LALT         , // 
					RightAltKey         = SDLK_RALT         , // 
					LeftMetaKey         = SDLK_LMETA        , // 
					RightMetaKey        = SDLK_RMETA        , // 
					
					// Left "Windows" key :
					LeftSuperKey        = SDLK_LSUPER       ,
					
					// Right "Windows" key : 
					RightSuperKey       = SDLK_RSUPER       , 
					ModeKey             = SDLK_MODE         , // "Alt Gr" key.
					
					// Multi-key compose key :
					ComposeKey          = SDLK_COMPOSE      ,  
					
					
					// Miscellaneous function keys :
					HelpKey             = SDLK_HELP         , // 
					PrintKey            = SDLK_PRINT        , // 
					SysReqKey           = SDLK_SYSREQ       , // 
					BreakKey            = SDLK_BREAK        , // 
					MenuKey             = SDLK_MENU         , // 
					
					// Power Macintosh power key :
					PowerKey            = SDLK_POWER        ,
					
					// Some european keyboards : 
					EuroKey             = SDLK_EURO         ,
					
					// Atari keyboard has Undo : 
					UndoKey             = SDLK_UNDO

	
					// Add any other keys here.

				} ;



				/**
				 * List of all currently available key modifiers 
				 * (control, alt, meta, etc.).
				 *
				 * They can be OR'd together, since modifiers can be 
				 * pressed simultaneously.
				 *
				 * @example : if ( mod == ( LeftShiftModifier 
				 *	| RightAltModifier ) )...
				 *
				 */
				enum KeyModifier
				{
					
					
					NoneModifier         = KMOD_NONE        ,
					
					LeftShiftModifier    = KMOD_LSHIFT      ,
					RightShiftModifier   = KMOD_RSHIFT      ,
					
					LeftControlModifier  = KMOD_LCTRL       ,
					RightControlModifier = KMOD_RCTRL       ,
					
					LeftAltModifier      = KMOD_LALT        ,
					RightAltModifier     = KMOD_RALT        ,
					
					LeftMetaModifier     = KMOD_LMETA       ,
					RightMetaModifier    = KMOD_RMETA       ,
					
					NumModifier          = KMOD_NUM         ,
					CapsModifier         = KMOD_CAPS        ,
					ModeModifier         = KMOD_MODE        ,
					ReservedModifier     = KMOD_RESERVED    ,
					
					
					/*
					 * Virtual modifiers, when left and right versions of 
					 * the modifier mean the same :
					 *
					 */
					ShiftModifier        = LeftShiftModifier   
						| RightShiftModifier,
						
					ControlModifier      = LeftControlModifier 
						| RightControlModifier,
						
					AltModifier          = LeftAltModifier     
						| RightAltModifier, 
						
					MetaModifier         = LeftMetaModifier    
						| RightMetaModifier
				
				
				} ;
				


				/**
				 * Constructs a new keyboard handler. 
				 *
				 * @param initialMode the mode in which this keyboard will
				 * start.
				 *
				 * @param useSmarterDefaultKeyHandler if true, a key handler
				 * that will exit the event loop if escape or 'q' is pressed,
				 * will be used. 
				 * Otherwise, a do-nothing key handler will be used.
				 *
				 * @note The keyboard mode is shared between all keyboard
				 * handlers.
				 *
				 * @throw InputDeviceHandlerException if keyboard could 
				 * not be initialized.
				 *
				 */
				explicit KeyboardHandler( KeyboardMode initialMode = rawInput, 
						bool useSmarterDefaultKeyHandler = false ) 
					throw( InputDeviceHandlerException ) ;
				
				
				/**
				 * Virtual destructor.
				 *
				 */
				virtual ~KeyboardHandler() throw() ;
				
								
				/**
				 * Links the specified raw key to specified controller, so 
				 * that any further key press or release of this raw key 
				 * will be sent to the controller, if the keyboard is in 
				 * raw input mode.
				 *
				 * Removes automatically any link previously defined 
				 * between this raw key and any other controller.
				 *
				 * @param rawKey the identifier of the raw key that shall 
				 * be linked to the controller.
				 *
				 * @param controller the OSDL controller which will be 
				 * notified of this raw key presses and releases.
				 *
				 */
				virtual void linkToController( KeyIdentifier rawKey, 
					OSDL::MVC::Controller & controller ) throw() ;
				
				
				/**
				 * Links the specified Unicode to specified controller, so 
				 * that any further selection of this Unicode will be 
				 * sent to the controller, if the keyboard is in text input
				 * mode.
				 *
				 * Removes automatically any link previously defined between
				 * this Unicode and any other controller.
				 *
				 * @param unicode the Unicode that shall be linked to the
				 * controller.
				 *
				 * @param controller the OSDL controller which will be 
				 * notified of this Unicode being selected.
				 *
				 */
				virtual void linkToController( Ceylan::Unicode unicode, 
					OSDL::MVC::Controller & controller ) throw() ;
				
				
				/**
				 * Links the specified raw key to specified keyboard event
				 * handler, so that if no controller is linked to this 
				 * raw key, the keyboard event handler is triggered.
				 *
				 * @param rawKey the raw key to link with the keyboard 
				 * event handler.
				 *
				 * @param handler the handler that will received the raw 
				 * key event, in the lack of linked controller.
				 *
				 */
				virtual void linkToHandler( KeyIdentifier rawKey,
					KeyboardEventHandler handler ) throw() ;
			
				
				/**
				 * Links the specified Unicode to specified keyboard event
				 * handler, so that if no controller is linked to this 
				 * Unicode, the keyboard event handler is triggered.
				 *
				 * @param unicode the Unicode to link with the keyboard 
				 * event handler.
				 *
				 * @param handler the handler that will received the 
				 * Unicode event, in the lack of linked controller.
				 *
				 */
				virtual void linkToHandler( Ceylan::Unicode unicode,
					KeyboardEventHandler handler ) throw() ;
				
				
				/**
				 * Called whenever a key was pressed, so that its 
				 * controller, if any, is notified.
				 *
				 * If no controller is registered for this key, then : 
				 *   - if a key handler is registered for this key, it will 
				 * be called
				 *   - otherwise (no handler for this key is found), then 
				 * the default key handler will be called. 
				 *
				 * @note The actions taken depend on the current input mode.
				 *
				 */
				virtual void keyPressed( const KeyboardEvent & keyboardEvent )
					throw() ;
				
				
				/**
				 * Called whenever a key was released, so that its controller,
				 * if any, is notified.
				 *
				 * If no controller is registered for this key, then : 
				 *   - if a key handler is registered for this key, it 
				 * will be called.
				 *   - otherwise (no handler for this key is found), then 
				 * the default key handler will be called. 
				 *
				 * @note The actions taken depend on the current input mode.
				 *
				 */
				virtual void keyReleased( const KeyboardEvent & keyboardEvent )
					throw() ;
				
				
				/**
				 * Sets the default key handlers to the smarter handlers.
				 *
				 * These handlers allow to trigger an exit from the event 
				 * loop if escape or 'q' key is pressed, both for raw and 
				 * input modes. 
				 *
				 * The 'F1' key is used to toggle between raw input and 
				 * text input keyboard modes.
				 *
				 */
				virtual void setSmarterDefaultKeyHandlers() throw() ;
				

				/**
				 * Sets a new default handler for raw key events.
				 *
				 * @param newHandler the new handler
				 *
				 */
				virtual void setDefaultRawKeyHandler( 
					KeyboardEventHandler newHandler ) throw() ;
				
				
				/**
				 * Sets a new default handler for Unicode events.
				 *
				 * @param newHandler the new handler
				 *
				 */
				virtual void setDefaultUnicodeHandler( 
					KeyboardEventHandler newHandler ) throw() ;
	
								
				/**
				 * Returns an user-friendly description of the state of 
				 * this object.
				 *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
				 *
				 */
		 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;



				// Static section :


				/**
				 * Returns the current keyboard mode.
				 *
				 * The keyboard at startup is in raw input mode.
				 *
				 * @return KeyboardMode the current keyboard mode.
				 *
				 * @note This belongs to overall common settings, it has
				 * therefore to be static.
				 *
				 */
				static KeyboardMode GetMode() throw() ;
				

				/**
				 * Sets the current keyboard mode.
				 *
				 * @param newMode the new keyboard mode.
				 *
				 * @note This belongs to overall common settings, it has
				 * therefore to be static.
				 *
				 */
				static void SetMode( KeyboardMode newMode ) throw() ;


				/**
				 * Describes specified key.
				 *
				 * @param key the key to describe.
				 *
				 */
				static std::string DescribeKey( KeyIdentifier key ) throw() ;
				
				
				/**
				 * Describes specified key modifier.
				 *
				 * @param modifier the modifier to describe 
				 * (alt, control, meta, etc.).
				 *
				 */
				static std::string DescribeModifier( KeyModifier modifier )
					throw() ;
				
				
				/**
				 * Describes specified Unicode value.
				 *
				 * @param value the Unicode value to describe.
				 *
				 */
				static std::string DescribeUnicode( Ceylan::Unicode value )
					throw() ;
				
				
				/**
				 * Specifies how long by default a key must be pressed
				 * before it begins repeating, in milliseconds.
				 *
				 */
				static const Ceylan::System::Millisecond
					DefaultDelayBeforeKeyRepeat ;
	
	
				/**
				 * Specifies how many milliseconds by default should be 
				 * waited until two repeated keys : once a key is hold long
				 * enough to repeat, this will be the time between two repeats
				 * of this key.
				 *
				 */
				static const Ceylan::System::Millisecond 
					DefaulKeyRepeatInterval ;
	
				 		

							
		protected :
		
				
				/// Stores the current keyboard interacting scheme.
				static KeyboardMode _CurrentMode ;
		
		
/* 
 * Takes care of the awful issue of Windows DLL with templates.
 *
 * @see Ceylan's developer guide and README-build-for-windows.txt 
 * to understand it, and to be aware of the associated risks. 
 * 
 */
#pragma warning( push )
#pragma warning( disable : 4251 )
		
		
				/** 
				 * Allows to link a controller to a specific keyboard raw 
				 * key, when in raw input mode.
				 *
				 * The controller can be then notified than one of its 
				 * tracked keys has been pressed or released.
				 *
				 * @note This is used only in raw input mode.
				 *
				 */
				std::map<KeyIdentifier, OSDL::MVC::Controller *>
					_rawKeyControllerMap ;
				
				
				/** 
				 * Fall-back map used whenever a raw key was pressed or 
				 * released without being registered in the map making 
				 * raw keys correspond to controllers : it may then be 
				 * linked to a specific keyboard event handler.
				 *
				 * When a key has been pressed or released, but no controller
				 * was linked to it, this key is sent to this default key
				 * dictionnary which manages external settings not linked 
				 * with MVC-based simulation world. 
				 *
				 * For example, an application could decide to quit when the
				 * so-defined 'q' key is pressed, which obviously should not
				 * rely on the controller MVC framework.
				 *
				 * @note This is used only in raw input mode.
				 *
				 */
				std::map<KeyIdentifier, KeyboardEventHandler> 
					_rawKeyHandlerMap ;
				
				
				/** 
				 * Allows to link a controller to a specific Unicode, 
				 * when in text input mode.
				 *
				 * The controller can be then notified than one of its 
				 * tracked unicodes has been selected by the user.
				 *
				 * @note This is used only in text input mode. 
				 * Linked controllers will only be notified in this mode 
				 * of key presses and repeats (not of key releases which are
				 * not relevant in this context).
				 *
				 */
				std::map<Ceylan::Unicode, OSDL::MVC::Controller *>
					_unicodeControllerMap ;
				
				
				/** 
				 * Fall-back map used whenever a Unicode was selected 
				 * by the user without being registered in the map making
				 * Unicodes correspond to controllers : it may then be 
				 * linked to a specific keyboard event handler.
				 *
				 * When a Unicode is selected, but no controller was linked 
				 * to it, this Unicode is sent to this default Unicode
				 * dictionnary which manages external settings not linked
				 * with the MVC-based simulation world. 
				 *
				 * For example, an application could decide to quit when 
				 * the 'q' Unicode is selected, which obviously should 
				 * not rely on the controller MVC framework.
				 *
				 * @note This is used only in text input mode. 
				 * Linked event handlers will only be notified in this 
				 * mode of key presses and repeats (not of key releases 
				 * which are not relevant in this context).
				 *
				 */
				std::map<Ceylan::Unicode, KeyboardEventHandler>
					_unicodeHandlerMap ;
				

#pragma warning( pop ) 



				/**
				 * Default raw key event handler, does nothing, but can 
				 * log keyboard events.
				 *
				 * @note It is only used when in raw input mode.
				 *
				 */
				KeyboardEventHandler _defaultRawKeyHandler ;
				
				
				/**
				 * Default Unicode event handler, does nothing, 
				 * but can log keyboard events.
				 *
				 * @note It is only used when in text input mode.
				 *
				 */
				KeyboardEventHandler _defaultUnicodeHandler ;
				
				
				/**
				 * Records the status of Unicode input before this handler 
				 * was created, so that it can be restored.
				 *
				 */
				bool _unicodeInputWasActivated ;



		private:
		
		
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit KeyboardHandler( const KeyboardHandler & source )
					throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				KeyboardHandler & operator = ( const KeyboardHandler & source )
					throw() ;
										
				
		} ;
	
	}	
	
}



#endif // OSDL_KEYBOARD_HANDLER_H_
