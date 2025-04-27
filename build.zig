const std = @import("std");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const vk = b.dependency("vk", .{});

    const spirv = b.addLibrary(.{
        .name = "spirv",
        .linkage = .static,
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        }),
    });
    spirv.linkLibCpp();

    spirv.addCSourceFiles(.{
        .root = b.path("vendor/spirv"),
        .files = &.{
            "spirv_module.cpp",
            "spirv_code_buffer.cpp",
        },
    });
    spirv.addIncludePath(b.path("vendor/spirv_cross"));
    b.installArtifact(spirv);

    const dxbc = b.addLibrary(.{
        .name = "dxbc",
        .linkage = .static,
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        }),
    });
    dxbc.linkLibCpp();
    dxbc.addCSourceFiles(.{
        .root = b.path("vendor/dxbc"),
        .files = &.{
            "dxbc_analysis.cpp",
            "dxbc_chunk_isgn.cpp",
            "dxbc_chunk_shex.cpp",
            "dxbc_common.cpp",
            "dxbc_compiler.cpp",
            "dxbc_decoder.cpp",
            "dxbc_defs.cpp",
            "dxbc_header.cpp",
            "dxbc_module.cpp",
            "dxbc_names.cpp",
            "dxbc_options.cpp",
            "dxbc_reader.cpp",
            "dxbc_util.cpp",
        },
    });
    dxbc.addCSourceFiles(.{
        .root = b.path("vendor/util"),
        .files = &.{
            "log/log.cpp",
            "log/log_debug.cpp",
            "util_env.cpp",
            "util_string.cpp",
        },
    });
    dxbc.addIncludePath(b.path("vendor/util"));
    dxbc.addIncludePath(b.path("vendor/spirv_cross"));
    dxbc.addIncludePath(vk.path("include"));
    dxbc.linkLibrary(spirv);
    b.installArtifact(dxbc);

    const sc = b.addLibrary(.{
        .name = "spirv_cross",
        .linkage = .static,
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        }),
    });
    sc.linkLibCpp();

    sc.addCSourceFiles(.{
        .root = b.path("vendor/spirv_cross"),
        .files = &.{
            // Core
            "spirv_cross.cpp",
            "spirv_parser.cpp",
            "spirv_cross_parsed_ir.cpp",
            "spirv_cfg.cpp",
            // GLSL
            "spirv_glsl.cpp",
            // HLSL
            "spirv_hlsl.cpp",
        },
        .flags = &.{
            "-DSPIRV_CROSS_C_API_GLSL=1",
            "-DSPIRV_CROSS_C_API_HLSL=1",
        },
    });
    b.installArtifact(sc);

    const driver = b.addLibrary(.{
        .linkage = .dynamic,
        .name = "dxbc2glsl",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        }),
    });
    driver.linkLibCpp();
    driver.addCSourceFile(.{ .file = b.path("src/driver.cpp") });
    driver.addIncludePath(b.path("vendor/dxbc"));
    driver.addIncludePath(b.path("vendor/util"));
    driver.addIncludePath(b.path("vendor"));
    driver.addIncludePath(vk.path("include"));
    driver.addIncludePath(b.path("vendor/spirv_cross"));

    driver.linkLibrary(dxbc);
    driver.linkLibrary(sc);
    driver.linkLibrary(spirv);
    b.installArtifact(driver);

    const translate_c = b.addTranslateC(.{
        .root_source_file = b.path("src/driver.h"),
        .target = target,
        .optimize = optimize,
    });

    const exe = b.addExecutable(.{
        .name = "translate",
        .target = target,
        .optimize = optimize,
        .root_source_file = b.path("src/translate.zig"),
    });
    exe.linkLibrary(driver);
    exe.root_module.addImport("d2g", translate_c.createModule());
    b.installArtifact(exe);

    const run = b.step("run", "");
    const run_artifact = b.addRunArtifact(exe);
    if (b.args) |args| run_artifact.addArgs(args);
    run.dependOn(&run_artifact.step);
}
