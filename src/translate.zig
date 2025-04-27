const std = @import("std");
const d2g = @import("d2g");

pub fn main() !void {
    const allocator = std.heap.smp_allocator;

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);
    if (args.len != 2) @panic("wrong amount of args");

    const file_path = args[1];
    const contents = try std.fs.cwd().readFileAlloc(
        allocator,
        file_path,
        100 * 1024 * 1024,
    );
    defer allocator.free(contents);

    const compiled = d2g.decompile_to_hlsl(contents.ptr, contents.len);
    defer d2g.free_compiled_string(compiled);

    std.debug.print("{s}\n", .{compiled});
}
