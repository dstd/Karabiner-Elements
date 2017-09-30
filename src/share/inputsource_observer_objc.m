#import "inputsource_observer_objc.h"
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

@interface KrbnInputsourceObserver : NSObject

@property krbn_inputsource_observer_callback callback;
@property void* context;

@end

@implementation KrbnInputsourceObserver

- (instancetype)initWithCallback:(krbn_inputsource_observer_callback)callback
                         context:(void*)context {
  self = [super init];

  if (self) {
    _callback = callback;
    _context = context;

    [[NSDistributedNotificationCenter defaultCenter] addObserver:self
                                                        selector:@selector(handleInputsourceChange:)
                                                            name:(NSString*)(kTISNotifySelectedKeyboardInputSourceChanged)
                                                          object:nil
                                              suspensionBehavior:NSNotificationSuspensionBehaviorDeliverImmediately];
  }

  return self;
}

- (void)dealloc {
  [[NSDistributedNotificationCenter defaultCenter] removeObserver:self];
}

- (void)handleInputsourceChange:(NSNotification*)notification {
  [self runCallbackWithInputsource];
}

- (void)runCallbackWithInputsource {
  if (!self.callback) {
    return;
  }
  
  TISInputSourceRef inputsource = TISCopyCurrentKeyboardLayoutInputSource();
  NSString* inputsource_id = (__bridge NSString*)TISGetInputSourceProperty(inputsource, kTISPropertyInputSourceID);
  
  dispatch_async(dispatch_get_main_queue(), ^{
    @try {
      const char* p = [inputsource_id UTF8String];
      if (self.callback) {
        self.callback(p ? p : "", self.context);
      }
    } @catch (NSException* exception) {
      NSLog(@"runCallback error");
    }
  });

  CFRelease(inputsource);
}

@end

void krbn_inputsource_observer_initialize(krbn_inputsource_observer_objc** observer,
                                          krbn_inputsource_observer_callback callback,
                                          void* context) {
  if (!observer) {
    NSLog(@"krbn_inputsource_observer_initialize invalid arguments");
    return;
  }
  if (*observer) {
    // observer is already initialized.
    return;
  }

  KrbnInputsourceObserver* o = [[KrbnInputsourceObserver alloc] initWithCallback:callback context:context];
  [o runCallbackWithInputsource];

  *observer = (__bridge_retained void*)(o);
}

void krbn_inputsource_observer_terminate(krbn_inputsource_observer_objc** observer) {
  if (observer) {
#ifndef __clang_analyzer__
    KrbnInputsourceObserver* o = (__bridge_transfer KrbnInputsourceObserver*)(*observer);
    o = nil;
#endif
  }
}
