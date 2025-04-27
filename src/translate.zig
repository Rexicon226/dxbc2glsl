const std = @import("std");
const d2g = @import("d2g");

const Output = enum { glsl, hlsl };

pub fn main() !void {
    const allocator = std.heap.smp_allocator;

    var args = try std.process.argsWithAllocator(allocator);
    defer args.deinit();

    var file_path: ?[]const u8 = null;
    var output: Output = .hlsl;

    while (args.next()) |arg| {
        if (std.mem.eql(u8, arg, "-g")) {
            output = .glsl;
        } else if (std.mem.eql(u8, arg, "-h")) {
            output = .hlsl;
        } else {
            if (file_path != null) return error.TwoFilesGiven;
            file_path = arg;
        }
    } else usage();

    const contents = try std.fs.cwd().readFileAlloc(
        allocator,
        file_path orelse return error.NoFileProvided,
        100 * 1024 * 1024,
    );
    defer allocator.free(contents);

    const compiled = switch (output) {
        .glsl => d2g.decompiled_to_glsl(contents.ptr, contents.len),
        .hlsl => d2g.decompiled_to_hlsl(contents.ptr, contents.len),
    };
    defer d2g.free_compiled_string(compiled);

    std.debug.print("{s}\n", .{compiled});
}

fn usage() noreturn {
    const stderr = std.io.getStdErr().writer();
    stderr.writeAll(
        \\translate [options] file
        \\
        \\Options: 
        \\  -g - Output GLSL
        \\  -h - Output HLSL [Default]
        \\
    ) catch @panic("failed to print usage");
    std.posix.exit(1);
}
