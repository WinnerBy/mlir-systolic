#!/usr/bin/env python3
"""
AutoSA HLS C Reference Generator

This script generates HLS C code for different kernels using AutoSA with various
spacetime, array_part, latency, and simd configurations. The generated files include
the AutoSA command as a comment at the beginning for reproducibility.
"""

import os
import sys
import subprocess
import json
import shutil
import argparse
from pathlib import Path
import re


# Kernel configurations with pre-defined parameters for kernels we want to generate
KERNELS = {
    'mm': {
        'path': 'autosa_tests/mm/kernel.c',
        'simd_info': 'autosa_tests/mm/simd_info.json',
        'spacetime_configs': {
            0: {'name': '1D_output_i', 'flags': []},
            1: {'name': '1D_output_j', 'flags': []},
            2: {'name': '1D_input_k', 'flags': ['--local-reduce', '--reduce-op="+"', '--simd-touch-space', '--array-contraction']},
            3: {'name': '2D_output_ij', 'flags': []},
            4: {'name': '2D_input_ik', 'flags': ['--local-reduce', '--reduce-op="+"', '--simd-touch-space', '--array-contraction']},
            5: {'name': '2D_input_jk', 'flags': ['--local-reduce', '--reduce-op="+"', '--simd-touch-space', '--array-contraction']},
        }
    },
    'cnn': {
        'path': 'autosa_tests/large/cnn/kernel.c',
        'simd_info': 'autosa_tests/large/cnn/simd_info.json',
        'spacetime_configs': {
            i: {'name': f'spacetime_{i}', 'flags': []}
            for i in range(8)  # CNN has ~8 spacetime configs (0-7)
        }
    },
    'lu': {
        'path': 'autosa_tests/lu/kernel.c',
        'simd_info': 'autosa_tests/lu/simd_info.json',
        'spacetime_configs': {
            0: {'name': 'spacetime_0', 'flags': ['--use-cplusplus-template', '--no-reschedule', '--live-range-reordering']},
            1: {'name': 'spacetime_1', 'flags': ['--use-cplusplus-template', '--no-reschedule', '--live-range-reordering']},
            2: {'name': 'spacetime_2', 'flags': ['--use-cplusplus-template', '--no-reschedule', '--live-range-reordering']},
            3: {'name': 'spacetime_3', 'flags': ['--use-cplusplus-template', '--no-reschedule', '--live-range-reordering']},
            4: {'name': 'spacetime_4', 'flags': ['--use-cplusplus-template', '--no-reschedule', '--live-range-reordering']},
            5: {'name': 'spacetime_5', 'flags': ['--use-cplusplus-template', '--no-reschedule', '--live-range-reordering']},
        }
    },
    'ttmc': {
        'path': 'autosa_tests/large/ttmc/kernel.c',
        'simd_info': 'autosa_tests/large/ttmc/simd_info.json',
        'spacetime_configs': {
            i: {'name': f'spacetime_{i}', 'flags': []}
            for i in range(10)  # TTMC has ~10 spacetime configs (0-9)
        }
    },
    'mttkrp': {
        'path': 'autosa_tests/large/mttkrp/kernel.c',
        'simd_info': 'autosa_tests/large/mttkrp/simd_info.json',
        'spacetime_configs': {
            i: {'name': f'spacetime_{i}', 'flags': []}
            for i in range(6)  # MTTKRP has ~6 spacetime configs (0-5)
        }
    }
}


class AutoSAGenerator:
    def __init__(self, autosa_root, output_root):
        self.autosa_root = Path(autosa_root)
        self.output_root = Path(output_root)
        self.autosa_bin = self.autosa_root / 'autosa'
        self.config_file = self.autosa_root / 'autosa_config' / 'autosa_config.json'
        self.tmp_dir = self.autosa_root / 'autosa.tmp'
        self.tmp_output_dir = self.tmp_dir / 'output'
        
        # Ensure output directory exists
        self.output_root.mkdir(parents=True, exist_ok=True)
        
    def ensure_tmp_structure(self):
        """Ensure the required temporary directory structure exists"""
        dirs = [
            self.tmp_dir,
            self.tmp_dir / 'output',
            self.tmp_dir / 'output' / 'src',
            self.tmp_dir / 'output' / 'latency_est',
            self.tmp_dir / 'output' / 'resource_est',
            self.tmp_dir / 'output' / 'tuning',
            self.tmp_dir / 'optimizer'
        ]
        for d in dirs:
            d.mkdir(parents=True, exist_ok=True)
    
    def clean_tmp_output(self):
        """Clean temporary output directory"""
        if self.tmp_output_dir.exists():
            for item in self.tmp_output_dir.iterdir():
                if item.is_file():
                    item.unlink()
                elif item.is_dir():
                    shutil.rmtree(item)
        self.ensure_tmp_structure()
    
    def run_autosa_explore(self, kernel_path, stage='space_time', verbose=False, kernel_config=None):
        """
        Run AutoSA to explore configuration options
        Returns the tuning.json content
        """
        self.clean_tmp_output()
        
        # Set config to manual mode for the specified stage
        config = self.load_config()
        if stage in config and isinstance(config[stage], dict):
            config[stage]['mode'] = 'manual'
        self.save_config(config)
        
        cmd = [
            str(self.autosa_bin),
            str(self.autosa_root / kernel_path),
            f'--config={self.config_file}',
            '--target=autosa_hls_c',
            f'--output-dir={self.tmp_output_dir}',
            '--host-serialize'
        ]
        
        # Add simd_info if available
        if kernel_config and kernel_config.get('simd_info'):
            cmd.append(f"--simd-info={self.autosa_root / kernel_config['simd_info']}")
        
        if verbose:
            cmd.append('--AutoSA-verbose')
        
        try:
            # Feed 'y' proactively to bypass reduction prompts
            # Must run from AutoSA root for relative paths to work
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                input='y\n' * 64,
                timeout=300,
                cwd=str(self.autosa_root)
            )
            
            # Read tuning.json
            tuning_file = self.tmp_output_dir / 'tuning' / 'tuning.json'
            if tuning_file.exists():
                with open(tuning_file, 'r') as f:
                    return json.load(f)
            return None
        except Exception as e:
            print(f"Error exploring {stage}: {e}")
            return None
    
    def run_autosa_generate(self, kernel_path, params, output_prefix, kernel_config=None):
        """
        Run AutoSA to generate HLS C code with specific parameters
        Returns (success, command, output_files)
        """
        self.clean_tmp_output()
        
        # Build command
        cmd = [
            str(self.autosa_bin),
            str(self.autosa_root / kernel_path),
            f'--config={self.config_file}',
            '--target=autosa_hls_c',
            f'--output-dir={self.tmp_output_dir}'
        ]

        # Always enable host serialization (better DRAM burst per tutorials)
        cmd.append('--host-serialize')

        # Attach simd_info if available to suppress reduction prompts
        if kernel_config and kernel_config.get('simd_info'):
            cmd.append(f"--simd-info={self.autosa_root / kernel_config['simd_info']}")
        
        # Add parameters
        if 'sa_sizes' in params:
            cmd.append(f'--sa-sizes={params["sa_sizes"]}')
        
        if 'extra_flags' in params:
            cmd.extend(params['extra_flags'])
        
        # Store the command
        cmd_str = ' '.join(cmd)
        
        try:
            # Run AutoSA (feed "y" proactively to bypass reduction prompts if any)
            # Must run from AutoSA root for relative paths to work
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                input='y\n' * 64,
                timeout=600,
                cwd=str(self.autosa_root)
            )
            
            if result.returncode != 0:
                print(f"AutoSA failed: {result.stderr}")
                return False, cmd_str, []
            
            # Find generated files
            src_dir = self.tmp_output_dir / 'src'
            output_files = []
            
            if src_dir.exists():
                for cpp_file in src_dir.glob('*.cpp'):
                    # Add command as comment to the beginning
                    content = cpp_file.read_text()
                    commented_cmd = f"// AutoSA Command:\n// {cmd_str}\n\n{content}"
                    
                    # Save to output directory
                    output_file = self.output_root / f"{output_prefix}_{cpp_file.name}"
                    output_file.write_text(commented_cmd)
                    output_files.append(output_file)
            
            return True, cmd_str, output_files
        
        except subprocess.TimeoutExpired:
            print(f"AutoSA timeout for {output_prefix}")
            return False, cmd_str, []
        except Exception as e:
            print(f"Error generating code: {e}")
            return False, cmd_str, []
    
    def load_config(self):
        """Load AutoSA config file"""
        with open(self.config_file, 'r') as f:
            return json.load(f)
    
    def save_config(self, config):
        """Save AutoSA config file"""
        with open(self.config_file, 'w') as f:
            json.dump(config, f, indent=2)
    
    def explore_spacetime(self, kernel_name, kernel_path, kernel_config):
        """Explore spacetime configurations for a kernel"""
        print(f"\n{'='*60}")
        print(f"Exploring spacetime for {kernel_name}")
        print(f"{'='*60}")
        
        tuning = self.run_autosa_explore(kernel_path, stage='space_time', verbose=True, kernel_config=kernel_config)
        
        if tuning and 'space_time' in tuning:
            num_arrays = tuning['space_time'].get('n_kernel', tuning['space_time'].get('num', 0))
            print(f"Found {num_arrays} spacetime configurations")
            
            configs = {}
            for i in range(num_arrays):
                configs[i] = {
                    'name': f'spacetime_{i}',
                    'flags': []  # Will be determined during generation
                }
            return configs
        
        print(f"No spacetime info in tuning.json or tuning exploration failed")
        return None
    
    def generate_kernel_variants(self, kernel_name, kernel_config, spacetime_id=None, 
                                 array_part=None, latency=None, simd=None):
        """
        Generate HLS C code variants for a kernel
        
        Args:
            kernel_name: Name of the kernel (e.g., 'mm')
            kernel_config: Configuration dict for the kernel
            spacetime_id: Specific spacetime ID (if None, will use predefined or explore)
            array_part: Array partitioning parameter
            latency: Latency hiding parameter
            simd: SIMD vectorization parameter
        """
        kernel_path = kernel_config['path']
        
        # Get spacetime configurations
        if kernel_config['spacetime_configs'] is None:
            spacetime_configs = self.explore_spacetime(kernel_name, kernel_path, kernel_config)
            if spacetime_configs is None:
                print(f"Failed to explore spacetime for {kernel_name}")
                return
        else:
            spacetime_configs = kernel_config['spacetime_configs']
        
        # Filter to specific spacetime if requested
        if spacetime_id is not None:
            if spacetime_id not in spacetime_configs:
                print(f"Spacetime ID {spacetime_id} not found for {kernel_name}")
                return
            spacetime_configs = {spacetime_id: spacetime_configs[spacetime_id]}
        
        # Generate variants
        results = []
        for st_id, st_config in spacetime_configs.items():
            print(f"\n--- Spacetime {st_id}: {st_config['name']} ---")
            
            # Build parameters
            params = {
                'sa_sizes': f'{{kernel[]->space_time[{st_id}]}}',
                'extra_flags': st_config['flags'].copy()
            }
            
            # Add array_part, latency, simd if specified
            if array_part:
                params['sa_sizes'] += f'; kernel[]->array_part[{array_part}]'
            if latency:
                params['sa_sizes'] += f'; kernel[]->latency[{latency}]'
            if simd:
                params['sa_sizes'] += f'; kernel[]->simd[{simd}]'
            
            # Generate output prefix
            output_prefix = f"{kernel_name}_st{st_id}"
            if array_part:
                output_prefix += f"_ap{array_part}"
            if latency:
                output_prefix += f"_lat{latency}"
            if simd:
                output_prefix += f"_simd{simd}"
            
            # Generate code
            success, cmd, files = self.run_autosa_generate(kernel_path, params, output_prefix, kernel_config)
            
            if success:
                print(f"✓ Generated: {', '.join([f.name for f in files])}")
                results.append({
                    'kernel': kernel_name,
                    'spacetime': st_id,
                    'config': st_config['name'],
                    'command': cmd,
                    'files': [str(f) for f in files]
                })
            else:
                print(f"✗ Failed to generate code")
        
        return results
    
    def generate_all_kernels(self, kernels=None):
        """Generate HLS C code for all or specified kernels"""
        if kernels is None:
            kernels = KERNELS.keys()
        
        all_results = []
        for kernel_name in kernels:
            if kernel_name not in KERNELS:
                print(f"Unknown kernel: {kernel_name}")
                continue
            
            kernel_config = KERNELS[kernel_name]
            results = self.generate_kernel_variants(kernel_name, kernel_config)
            if results:
                all_results.extend(results)
        
        # Save summary
        summary_file = self.output_root / 'generation_summary.json'
        with open(summary_file, 'w') as f:
            json.dump(all_results, f, indent=2)
        
        print(f"\n{'='*60}")
        print(f"Generation complete. Summary saved to {summary_file}")
        print(f"Total files generated: {sum(len(r['files']) for r in all_results)}")
        print(f"{'='*60}")

    def generate_with_params(self, kernels=None):
        """
        Generate with predefined parameter combinations.
        Uses parameter values from gen_final.sh for reproducibility.
        """
        if kernels is None:
            kernels = list(KERNELS.keys())
        elif isinstance(kernels, str):
            kernels = [kernels]
        
        # Define parameter combinations per kernel
        # DIVERSITY FIRST: For each spacetime, select 2-3 representative parameter variants
        # Aim: ~88 total files with diverse kernel types, spacetimes, and parameters
        param_combos = {
            # MM: 6 spacetimes × 3 parameter variants + 1 extra = 19 files
            'mm': [
                # Spacetime 0: 1D_output_i (3 param variants: small, medium, large)
                (0, '16,16,16', '8,8', '2'),
                (0, '32,32,32', '16,16', '4'),
                (0, '64,64,64', '32,32', '8'),
                # Spacetime 1: 1D_output_j
                (1, '16,16,16', '8,8', '2'),
                (1, '32,32,32', '16,16', '4'),
                (1, '64,64,64', '32,32', '8'),
                # Spacetime 2: 1D_input_k (requires --local-reduce)
                (2, '16,16,16', '8,8', '2'),
                (2, '32,32,32', '16,16', '4'),
                (2, '64,64,64', '32,32', '8'),
                # Spacetime 3: 2D_output_ij
                (3, '16,16,16', '8,8', '2'),
                (3, '32,32,32', '16,16', '4'),
                (3, '64,64,64', '32,32', '8'),
                # Spacetime 4: 2D_input_ik (requires --local-reduce)
                (4, '16,16,16', '8,8', '2'),
                (4, '32,32,32', '16,16', '4'),
                (4, '64,64,64', '32,32', '8'),
                # Spacetime 5: 2D_input_jk (requires --local-reduce)
                (5, '16,16,16', '8,8', '2'),
                (5, '32,32,32', '16,16', '4'),
                (5, '64,64,64', '32,32', '8'),
                # Extra MM variant (safe size)
                (5, '48,48,48', '24,24', '6'),
            ],  # MM total: 19
            
            # TTMC: 6 spacetimes × 3-4 parameter variants + 1 extra = 21 files
            'ttmc': [
                # Spacetime 0 (3 param variants)
                (0, '16,16,16,16', '1,2,2', '1,1'),
                (0, '32,32,32,32', '2,4,4', '2,2'),
                (0, '64,64,64,64', '4,8,8', '4,4'),
                # Spacetime 1
                (1, '16,16,16,16', '1,2,2', '1,1'),
                (1, '32,32,32,32', '2,4,4', '2,2'),
                (1, '64,64,64,64', '4,8,8', '4,4'),
                # Spacetime 2 - skip the failing st2_small, use medium and large
                (2, '32,32,32,32', '2,4,4', '2,2'),
                (2, '64,64,64,64', '4,8,8', '4,4'),
                # Spacetime 3
                (3, '16,16,16,16', '1,2,2', '1,1'),
                (3, '32,32,32,32', '2,4,4', '2,2'),
                (3, '64,64,64,64', '4,8,8', '4,4'),
                (3, '128,128,128,128', '8,16,16', '8,8'),
                # Spacetime 4
                (4, '16,16,16,16', '1,2,2', '1,1'),
                (4, '32,32,32,32', '2,4,4', '2,2'),
                (4, '64,64,64,64', '4,8,8', '4,4'),
                (4, '128,128,128,128', '8,16,16', '8,8'),
                # Spacetime 5
                (5, '16,16,16,16', '1,2,2', '1,1'),
                (5, '32,32,32,32', '2,4,4', '2,2'),
                (5, '64,64,64,64', '4,8,8', '4,4'),
                (5, '128,128,128,128', '8,16,16', '8,8'),
                # Spacetime 6 (add medium and large)
                (6, '32,32,32,32', '2,4,4', '2,2'),
                (6, '64,64,64,64', '4,8,8', '4,4'),
                # Spacetime 7 (add medium and large)
                (7, '32,32,32,32', '2,4,4', '2,2'),
                (7, '64,64,64,64', '4,8,8', '4,4'),
                # Spacetime 8 (add medium and large)
                (8, '32,32,32,32', '2,4,4', '2,2'),
                (8, '64,64,64,64', '4,8,8', '4,4'),
                # Spacetime 9 (add medium and large)
                (9, '32,32,32,32', '2,4,4', '2,2'),
                (9, '64,64,64,64', '4,8,8', '4,4'),
                # Extra TTMC variant  
                (0, '128,128,128,128', '8,16,16', '8,8'),
            ],  # TTMC total: 21
            
            # MTTKRP: 4 spacetimes × 4 parameter variants + 2 extra = 18 files
            'mttkrp': [
                # Spacetime 0 (4 param variants: small, small-med, medium, large)
                (0, '32,32,2', '4,2', '2,1'),
                (0, '64,64,2', '8,4', '4,1'),
                (0, '128,128,2', '16,8', '8,1'),
                (0, '256,256,4', '32,16', '16,2'),
                # Spacetime 1
                (1, '32,32,2', '4,2', '2,1'),
                (1, '64,64,2', '8,4', '4,1'),
                (1, '128,128,2', '16,8', '8,1'),
                (1, '256,256,4', '32,16', '16,2'),
                # Spacetime 2
                (2, '32,32,2', '4,2', '2,1'),
                (2, '64,64,2', '8,4', '4,1'),
                (2, '128,128,2', '16,8', '8,1'),
                (2, '256,256,4', '32,16', '16,2'),
                # Spacetime 3
                (3, '32,32,2', '4,2', '2,1'),
                (3, '64,64,2', '8,4', '4,1'),
                (3, '128,128,2', '16,8', '8,1'),
                (3, '256,256,4', '32,16', '16,2'),
                # Spacetime 4 (add medium, large, extra-large)
                (4, '64,64,2', '8,4', '4,1'),
                (4, '128,128,2', '16,8', '8,1'),
                (4, '256,256,4', '32,16', '16,2'),
                # Spacetime 5 (add medium, large, extra-large)
                (5, '64,64,2', '8,4', '4,1'),
                (5, '128,128,2', '16,8', '8,1'),
                (5, '256,256,4', '32,16', '16,2'),
                # Extra MTTKRP variants (removed unstable 96-size variants)
            ],  # MTTKRP total: 18
            
            # LU: 6 spacetimes × 2 parameter variants = 12 files
            'lu': [
                # LU uses array_part[-1,-1,-1] (no partition)
                # Spacetime 0 (2 variants: different latency settings)
                (0, '-1,-1,-1', '4,4', '1,1'),
                (0, '-1,-1,-1', '8,8', '2,2'),
                # Spacetime 1
                (1, '-1,-1,-1', '4,4', '1,1'),
                (1, '-1,-1,-1', '8,8', '2,2'),
                # Spacetime 2
                (2, '-1,-1,-1', '4,4', '1,1'),
                (2, '-1,-1,-1', '8,8', '2,2'),
                # Spacetime 3 (standard from README)
                (3, '-1,-1,-1', '4,4', '1,1'),
                (3, '-1,-1,-1', '8,8', '2,2'),
                # Spacetime 4
                (4, '-1,-1,-1', '4,4', '1,1'),
                (4, '-1,-1,-1', '8,8', '2,2'),
                # Spacetime 5
                (5, '-1,-1,-1', '4,4', '1,1'),
                (5, '-1,-1,-1', '8,8', '2,2'),
            ],  # LU total: 12
            
            # CNN: 8 spacetimes × 3 parameter variants = 24 files  
            'cnn': [
                # CNN needs 4D array_part: [O, R, C, I]
                # Spacetime 0 (3 param variants: small, medium, large)
                (0, '64,56,14,64', '4,4,7', '1,1,8'),
                (0, '96,84,21,96', '6,6,10', '1,1,12'),
                (0, '128,112,28,128', '8,8,14', '2,2,16'),
                # Spacetime 1
                (1, '64,56,14,64', '4,4,7', '1,1,8'),
                (1, '96,84,21,96', '6,6,10', '1,1,12'),
                (1, '128,112,28,128', '8,8,14', '2,2,16'),
                # Spacetime 2
                (2, '64,56,14,64', '4,4,7', '1,1,8'),
                (2, '96,84,21,96', '6,6,10', '1,1,12'),
                (2, '128,112,28,128', '8,8,14', '2,2,16'),
                # Spacetime 3
                (3, '64,56,14,64', '4,4,7', '1,1,8'),
                (3, '96,84,21,96', '6,6,10', '1,1,12'),
                (3, '128,112,28,128', '8,8,14', '2,2,16'),
                # Spacetime 4
                (4, '64,56,14,64', '4,4,7', '1,1,8'),
                (4, '96,84,21,96', '6,6,10', '1,1,12'),
                (4, '128,112,28,128', '8,8,14', '2,2,16'),
                # Spacetime 5
                (5, '64,56,14,64', '4,4,7', '1,1,8'),
                (5, '96,84,21,96', '6,6,10', '1,1,12'),
                (5, '128,112,28,128', '8,8,14', '2,2,16'),
                # Spacetime 6
                (6, '64,56,14,64', '4,4,7', '1,1,8'),
                (6, '96,84,21,96', '6,6,10', '1,1,12'),
                (6, '128,112,28,128', '8,8,14', '2,2,16'),
                # Spacetime 7
                (7, '64,56,14,64', '4,4,7', '1,1,8'),
                (7, '96,84,21,96', '6,6,10', '1,1,12'),
                (7, '128,112,28,128', '8,8,14', '2,2,16'),
                # Additional 8 safe variants (spacetimes 0-7 with different parameter)
                (0, '80,70,17,80', '5,5,8', '1,1,10'),
                (1, '80,70,17,80', '5,5,8', '1,1,10'),
                (2, '80,70,17,80', '5,5,8', '1,1,10'),
                (3, '80,70,17,80', '5,5,8', '1,1,10'),
                (4, '80,70,17,80', '5,5,8', '1,1,10'),
                (5, '80,70,17,80', '5,5,8', '1,1,10'),
                (6, '80,70,17,80', '5,5,8', '1,1,10'),
                (7, '80,70,17,80', '5,5,8', '1,1,10'),
            ],  # CNN total: 32
        }
        
        # Summary by kernel type and diversity:
        # MM:      6 spacetimes × 3 parameter variants + 1 extra = 19 files (diverse parameters per spacetime)
        # TTMC:    6 spacetimes × 3-4 param variants + 1 extra = 21 files (small, medium, large, extra-large)
        # MTTKRP:  4 spacetimes × 4 parameter variants + 2 extra = 18 files (4 scale levels)
        # LU:      6 spacetimes × 2 parameter variants = 12 files (different latency settings)
        # CNN:     8 spacetimes × 3 parameter variants = 24 files (4D array structures)
        # TOTAL:                                        = 94 files (6 beyond 88 target)
        # 
        # Diversity focus: Each kernel has multiple spacetime configurations
        # Each spacetime has 2-4 different parameter variants (small/medium/large scales)
        # This provides reference examples for different synthesis strategies
        
        all_results = []
        
        for kernel_name in kernels:
            if kernel_name not in KERNELS:
                print(f"Unknown kernel: {kernel_name}")
                continue
            
            if kernel_name not in param_combos:
                print(f"No parameter combos defined for {kernel_name}")
                continue
            
            kernel_config = KERNELS[kernel_name]
            kernel_path = kernel_config['path']
            combos = param_combos[kernel_name]
            
            print(f"\n{'='*60}")
            print(f"Generating {kernel_name} with parameters")
            print(f"{'='*60}")
            
            for st_id, array_part, latency, simd in combos:
                output_dir = self.output_root / kernel_name / f"st{st_id}_ap{array_part}_lat{latency}_simd{simd}"
                
                # Skip if already exists
                if (output_dir / 'kernel_kernel.cpp').exists():
                    print(f"[skip] {kernel_name}/st{st_id}_ap{array_part}_lat{latency}_simd{simd}")
                    continue
                
                # Build parameters
                params = {
                    'sa_sizes': f'{{kernel[]->space_time[{st_id}];kernel[]->array_part[{array_part}];kernel[]->latency[{latency}];kernel[]->simd[{simd}]}}',
                    'extra_flags': kernel_config['spacetime_configs'][st_id]['flags'].copy()
                }
                
                output_prefix = f"{kernel_name}_st{st_id}_ap{array_part}_lat{latency}_simd{simd}"
                
                # Generate code
                success, cmd, files = self.run_autosa_generate(kernel_path, params, output_prefix, kernel_config)
                
                if success:
                    # Move files to proper location
                    output_dir.mkdir(parents=True, exist_ok=True)
                    for src_file in files:
                        src_path = Path(src_file)
                        dst_file = output_dir / src_path.name.replace(output_prefix + '_', '')
                        src_path.rename(dst_file)
                        print(f"[✓] {kernel_name}/st{st_id}_ap{array_part}_lat{latency}_simd{simd}/{dst_file.name}")
                    
                    all_results.append({
                        'kernel': kernel_name,
                        'spacetime': st_id,
                        'array_part': array_part,
                        'latency': latency,
                        'simd': simd,
                        'command': cmd,
                        'files': [str(output_dir / f.name) for f in files]
                    })
                else:
                    print(f"[✗] {kernel_name}/st{st_id}_ap{array_part}_lat{latency}_simd{simd}")
        
        # Save summary
        summary_file = self.output_root / 'generation_summary.json'
        with open(summary_file, 'w') as f:
            json.dump(all_results, f, indent=2)
        
        print(f"\n{'='*60}")
        print(f"Generation complete. Summary saved to {summary_file}")
        print(f"Total files generated: {sum(len(r['files']) for r in all_results)}")
        print(f"{'='*60}")


def main():
    parser = argparse.ArgumentParser(
        description='Generate AutoSA HLS C reference code for different kernels'
    )
    parser.add_argument(
        '--autosa-root',
        default='/workspaces/mlir-systolic/third_party/AutoSA',
        help='Path to AutoSA root directory'
    )
    parser.add_argument(
        '--output-dir',
        default='/workspaces/mlir-systolic/autosa_hls_refs',
        help='Output directory for generated HLS C files'
    )
    parser.add_argument(
        '--kernels',
        nargs='+',
        choices=list(KERNELS.keys()),
        help='Specific kernels to generate (default: all)'
    )
    parser.add_argument(
        '--spacetime',
        type=int,
        help='Specific spacetime configuration ID'
    )
    parser.add_argument(
        '--with-params',
        action='store_true',
        help='Generate with array_part, latency, simd parameters (uses predefined combos)'
    )
    
    args = parser.parse_args()
    
    # Initialize generator
    generator = AutoSAGenerator(args.autosa_root, args.output_dir)
    generator.ensure_tmp_structure()
    
    # Generate code
    if args.with_params:
        # Use parameter-based generation with predefined combos
        print("Generating with predefined parameter combinations...")
        generator.generate_with_params(args.kernels)
    else:
        # Basic generation with just spacetime
        if args.kernels:
            generator.generate_all_kernels(args.kernels)
        else:
            generator.generate_all_kernels()


if __name__ == '__main__':
    main()
