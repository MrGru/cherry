#import "AppRenderer.h"
#import "AppViewController.h"
#import "AppView.h"
#import <cherry/graphic/graphic.h>
#import <cherry/graphic/metal.h>
#import <cherry/game/game.h>
#import <cherry/memory.h>

static const long kInFlightCommandBuffers = BUFFERS;

@implementation AppRenderer
{
    dispatch_semaphore_t _inflight_semaphore;
    id <MTLDevice> _device;
    id <MTLCommandQueue> _commandQueue;
    struct game *game;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        _inflight_semaphore = dispatch_semaphore_create(kInFlightCommandBuffers);
    }
    return self;
}

#pragma mark Configure

- (void)configure:(AppView *)view
{
    _device = view.device;
    
    shared_mtl_device = view.device;
    
    // setup view with drawable formats
    view.depthPixelFormat   = MTLPixelFormatDepth32Float;
    view.stencilPixelFormat = MTLPixelFormatInvalid;
    view.sampleCount        = 1;
    
    // create a new command queue
    _commandQueue = [_device newCommandQueue];
    
    shared_mtl_library = [_device newDefaultLibrary];
    
    shared_mtl_main_pass = view.renderPassDescriptor;
    game = game_alloc();
}

#pragma mark Render

- (void)render:(AppView *)view
{
    if(!game) return;
    
    dispatch_semaphore_wait(_inflight_semaphore, DISPATCH_TIME_FOREVER);
    
    id <MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];
    shared_mtl_command_buffer = commandBuffer;
    shared_mtl_main_pass = view.renderPassDescriptor;
    
    game_render(game);
    [commandBuffer presentDrawable:view.currentDrawable];
    

    __block dispatch_semaphore_t block_sema = _inflight_semaphore;
    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(block_sema);
    }];
    
    [commandBuffer commit];
}

- (void)reshape:(AppView *)view
{
    if(game) {
        game_resize(game, view.bounds.size.width, view.bounds.size.height);
    }
}

#pragma mark Update

- (void)update:(AppViewController *)controller
{
    if(!game) return;
    
    game_update(game);
}

- (void)viewController:(AppViewController *)controller willPause:(BOOL)pause
{
    
}

-(void)destroyGame
{
    if(game) {
        game_free(game);
        game = NULL;
        
        cache_free();
        dim_memory();
    }
}


@end
