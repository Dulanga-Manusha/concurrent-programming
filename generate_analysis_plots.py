#!/usr/bin/env python3
"""
Performance Analysis Plotting Script
Generates additional graphs to support critical evaluation of concurrent linked list results
"""

import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from matplotlib.patches import Rectangle
import pandas as pd

# Set style for professional plots
plt.style.use('default')
sns.set_palette("husl")

# Performance data from experiments
data = {
    'case1': {
        'serial': [0.007689, 0.001049],
        'mutex': [(0.007745, 0.001544), (0.020138, 0.002252), (0.024151, 0.005480), (0.030317, 0.003091)],
        'rwlock': [(0.007277, 0.000640), (0.006366, 0.000530), (0.007565, 0.000528), (0.008276, 0.000857)]
    },
    'case2': {
        'serial': [0.008681, 0.001096],
        'mutex': [(0.009053, 0.000841), (0.027447, 0.002551), (0.032243, 0.001477), (0.052234, 0.010012)],
        'rwlock': [(0.011038, 0.001436), (0.039368, 0.001524), (0.042728, 0.001711), (0.045779, 0.001744)]
    },
    'case3': {
        'serial': [0.029550, 0.001923],
        'mutex': [(0.032718, 0.001686), (0.110628, 0.030816), (0.164749, 0.014709), (0.145981, 0.016789)],
        'rwlock': [(0.035515, 0.003888), (0.141426, 0.016199), (0.147349, 0.007846), (0.195080, 0.032478)]
    }
}

threads = [1, 2, 4, 8]

def create_performance_comparison():
    """Create comprehensive performance comparison plots"""
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    fig.suptitle('Concurrent Linked List Performance Analysis\nCritical Evaluation with Error Bars', fontsize=16, fontweight='bold')
    
    cases = ['case1', 'case2', 'case3']
    case_titles = ['Case 1: 99% Member (Read-Heavy)', 'Case 2: 90% Member (Mixed)', 'Case 3: 50% Member (Write-Heavy)']
    
    for i, (case, title) in enumerate(zip(cases, case_titles)):
        # Performance vs Thread Count
        ax1 = axes[0, i]
        
        # Extract data
        mutex_means = [data[case]['mutex'][j][0] for j in range(4)]
        mutex_stds = [data[case]['mutex'][j][1] for j in range(4)]
        rwlock_means = [data[case]['rwlock'][j][0] for j in range(4)]
        rwlock_stds = [data[case]['rwlock'][j][1] for j in range(4)]
        serial_mean = data[case]['serial'][0]
        
        # Plot with error bars
        ax1.errorbar(threads, mutex_means, yerr=mutex_stds, marker='o', linewidth=2, 
                    markersize=8, label='Mutex', capsize=5, capthick=2)
        ax1.errorbar(threads, rwlock_means, yerr=rwlock_stds, marker='s', linewidth=2, 
                    markersize=8, label='Read-Write Lock', capsize=5, capthick=2)
        ax1.axhline(y=serial_mean, color='red', linestyle='--', linewidth=2, 
                   label='Serial Baseline', alpha=0.8)
        
        # Highlight anomalies
        if case == 'case1':
            # Highlight the "speedup" anomaly
            ax1.annotate('Anomaly: 21% "speedup"', 
                        xy=(2, rwlock_means[1]), xytext=(3, rwlock_means[1]-0.001),
                        arrowprops=dict(arrowstyle='->', color='red', lw=2),
                        fontsize=10, color='red', fontweight='bold')
        
        if case == 'case3':
            # Highlight the performance reversal
            ax1.annotate('Critical Anomaly:\nPerformance Reversal', 
                        xy=(6, mutex_means[3]), xytext=(6.5, mutex_means[2]),
                        arrowprops=dict(arrowstyle='->', color='red', lw=2),
                        fontsize=10, color='red', fontweight='bold')
        
        ax1.set_xlabel('Number of Threads', fontweight='bold')
        ax1.set_ylabel('Execution Time (seconds)', fontweight='bold')
        ax1.set_title(title, fontweight='bold')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        ax1.set_xticks(threads)
        
        # Speedup Analysis
        ax2 = axes[1, i]
        
        mutex_speedup = [serial_mean / mutex_means[j] for j in range(4)]
        rwlock_speedup = [serial_mean / rwlock_means[j] for j in range(4)]
        
        ax2.plot(threads, mutex_speedup, marker='o', linewidth=2, markersize=8, 
                label='Mutex Speedup', color='orange')
        ax2.plot(threads, rwlock_speedup, marker='s', linewidth=2, markersize=8, 
                label='RW-Lock Speedup', color='green')
        ax2.axhline(y=1.0, color='black', linestyle='-', alpha=0.5, label='Ideal (1.0x)')
        
        # Highlight problematic speedups
        if case == 'case1':
            ax2.annotate('Theoretical Issue:\nSpeedup > 1.0x', 
                        xy=(2, rwlock_speedup[1]), xytext=(3.5, rwlock_speedup[1]+0.1),
                        arrowprops=dict(arrowstyle='->', color='red', lw=2),
                        fontsize=9, color='red', fontweight='bold')
        
        ax2.set_xlabel('Number of Threads', fontweight='bold')
        ax2.set_ylabel('Speedup (Serial Time / Parallel Time)', fontweight='bold')
        ax2.set_title(f'Speedup Analysis - {title.split(":")[0]}', fontweight='bold')
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        ax2.set_xticks(threads)
        ax2.set_ylim(0, max(max(mutex_speedup), max(rwlock_speedup)) * 1.1)
    
    plt.tight_layout()
    plt.savefig('performance_critical_analysis.png', dpi=300, bbox_inches='tight')
    plt.show()

def create_variability_analysis():
    """Create coefficient of variation analysis"""
    fig, axes = plt.subplots(1, 3, figsize=(18, 6))
    fig.suptitle('Statistical Reliability Analysis - Coefficient of Variation', fontsize=16, fontweight='bold')
    
    cases = ['case1', 'case2', 'case3']
    case_titles = ['Case 1: Read-Heavy', 'Case 2: Mixed', 'Case 3: Write-Heavy']
    
    for i, (case, title) in enumerate(zip(cases, case_titles)):
        ax = axes[i]
        
        # Calculate coefficient of variation (CV = std/mean)
        mutex_cv = [(data[case]['mutex'][j][1] / data[case]['mutex'][j][0]) * 100 for j in range(4)]
        rwlock_cv = [(data[case]['rwlock'][j][1] / data[case]['rwlock'][j][0]) * 100 for j in range(4)]
        
        x = np.arange(len(threads))
        width = 0.35
        
        bars1 = ax.bar(x - width/2, mutex_cv, width, label='Mutex', alpha=0.8, color='orange')
        bars2 = ax.bar(x + width/2, rwlock_cv, width, label='Read-Write Lock', alpha=0.8, color='green')
        
        # Highlight high variability (>15% CV)
        for j, cv in enumerate(mutex_cv):
            if cv > 15:
                bars1[j].set_color('red')
                ax.annotate(f'High Variability\n{cv:.1f}%', 
                           xy=(j - width/2, cv), xytext=(j - width/2, cv + 5),
                           arrowprops=dict(arrowstyle='->', color='red'),
                           fontsize=9, color='red', ha='center', fontweight='bold')
        
        for j, cv in enumerate(rwlock_cv):
            if cv > 15:
                bars2[j].set_color('red')
                ax.annotate(f'High Variability\n{cv:.1f}%', 
                           xy=(j + width/2, cv), xytext=(j + width/2, cv + 5),
                           arrowprops=dict(arrowstyle='->', color='red'),
                           fontsize=9, color='red', ha='center', fontweight='bold')
        
        ax.axhline(y=10, color='red', linestyle='--', alpha=0.5, label='10% Threshold')
        ax.set_xlabel('Number of Threads', fontweight='bold')
        ax.set_ylabel('Coefficient of Variation (%)', fontweight='bold')
        ax.set_title(title, fontweight='bold')
        ax.set_xticks(x)
        ax.set_xticklabels(threads)
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('variability_analysis.png', dpi=300, bbox_inches='tight')
    plt.show()

def create_anomaly_heatmap():
    """Create heatmap showing performance anomalies"""
    fig, ax = plt.subplots(1, 1, figsize=(12, 8))
    
    # Create anomaly severity matrix
    anomaly_data = []
    labels = []
    
    for case in ['case1', 'case2', 'case3']:
        for impl in ['mutex', 'rwlock']:
            row = []
            for j in range(4):
                if impl == 'mutex':
                    mean, std = data[case][impl][j]
                else:
                    mean, std = data[case][impl][j]
                
                # Calculate anomaly score
                cv = (std / mean) * 100
                
                # Special cases
                anomaly_score = cv
                if case == 'case1' and impl == 'rwlock' and j == 1:  # RW-lock speedup
                    anomaly_score += 20  # Boost for theoretical inconsistency
                if case == 'case3' and impl == 'mutex' and j == 3:  # Performance reversal
                    anomaly_score += 30  # High boost for critical anomaly
                
                row.append(anomaly_score)
            
            anomaly_data.append(row)
            labels.append(f'{case.upper()}-{impl.upper()}')
    
    # Create heatmap
    im = ax.imshow(anomaly_data, cmap='RdYlBu_r', aspect='auto')
    
    # Set ticks and labels
    ax.set_xticks(range(4))
    ax.set_xticklabels(['1 Thread', '2 Threads', '4 Threads', '8 Threads'])
    ax.set_yticks(range(len(labels)))
    ax.set_yticklabels(labels)
    
    # Add colorbar
    cbar = plt.colorbar(im)
    cbar.set_label('Anomaly Severity Score', rotation=270, labelpad=20, fontweight='bold')
    
    # Add text annotations
    for i in range(len(labels)):
        for j in range(4):
            text = ax.text(j, i, f'{anomaly_data[i][j]:.1f}', 
                          ha="center", va="center", color="black", fontweight='bold')
    
    ax.set_title('Performance Anomaly Detection Heatmap\n(Higher scores indicate more concerning results)', 
                fontweight='bold', pad=20)
    
    plt.tight_layout()
    plt.savefig('anomaly_heatmap.png', dpi=300, bbox_inches='tight')
    plt.show()

def create_theoretical_vs_actual():
    """Compare theoretical expectations vs actual results"""
    fig, axes = plt.subplots(1, 3, figsize=(18, 6))
    fig.suptitle('Theoretical vs Actual Performance Comparison', fontsize=16, fontweight='bold')
    
    cases = ['case1', 'case2', 'case3']
    case_titles = ['Case 1: Read-Heavy', 'Case 2: Mixed', 'Case 3: Write-Heavy']
    
    for i, (case, title) in enumerate(zip(cases, case_titles)):
        ax = axes[i]
        
        # Actual performance
        mutex_actual = [data[case]['mutex'][j][0] for j in range(4)]
        rwlock_actual = [data[case]['rwlock'][j][0] for j in range(4)]
        serial_time = data[case]['serial'][0]
        
        # Theoretical expectations (simplified model)
        # For read-heavy: RW-locks should scale well, mutex should degrade
        # For write-heavy: Both should degrade significantly
        if case == 'case1':  # Read-heavy
            mutex_theoretical = [serial_time * (1 + 0.1 * (t-1)) for t in threads]  # Slight degradation
            rwlock_theoretical = [serial_time / (1 + 0.1 * (t-1)) for t in threads]  # Slight improvement
        elif case == 'case2':  # Mixed
            mutex_theoretical = [serial_time * (1 + 0.2 * (t-1)) for t in threads]  # Moderate degradation
            rwlock_theoretical = [serial_time * (1 + 0.15 * (t-1)) for t in threads]  # Slight degradation
        else:  # Write-heavy
            mutex_theoretical = [serial_time * (1 + 0.3 * (t-1)) for t in threads]  # Significant degradation
            rwlock_theoretical = [serial_time * (1 + 0.4 * (t-1)) for t in threads]  # More degradation
        
        # Plot theoretical vs actual
        ax.plot(threads, mutex_theoretical, '--', color='orange', linewidth=2, 
               label='Mutex (Theoretical)', alpha=0.7)
        ax.plot(threads, mutex_actual, '-o', color='orange', linewidth=3, 
               markersize=8, label='Mutex (Actual)')
        
        ax.plot(threads, rwlock_theoretical, '--', color='green', linewidth=2, 
               label='RW-Lock (Theoretical)', alpha=0.7)
        ax.plot(threads, rwlock_actual, '-s', color='green', linewidth=3, 
               markersize=8, label='RW-Lock (Actual)')
        
        # Highlight significant deviations
        for j, (actual, theoretical) in enumerate(zip(mutex_actual, mutex_theoretical)):
            deviation = abs(actual - theoretical) / theoretical
            if deviation > 0.3:  # >30% deviation
                ax.annotate(f'Deviation:\n{deviation*100:.0f}%', 
                           xy=(threads[j], actual), xytext=(threads[j]+0.3, actual),
                           arrowprops=dict(arrowstyle='->', color='red'),
                           fontsize=8, color='red')
        
        ax.set_xlabel('Number of Threads', fontweight='bold')
        ax.set_ylabel('Execution Time (seconds)', fontweight='bold')
        ax.set_title(title, fontweight='bold')
        ax.legend()
        ax.grid(True, alpha=0.3)
        ax.set_xticks(threads)
    
    plt.tight_layout()
    plt.savefig('theoretical_vs_actual.png', dpi=300, bbox_inches='tight')
    plt.show()

if __name__ == "__main__":
    print("Generating critical analysis plots...")
    
    # Create all plots
    create_performance_comparison()
    create_variability_analysis() 
    create_anomaly_heatmap()
    create_theoretical_vs_actual()
    
    print("\nPlots generated:")
    print("1. performance_critical_analysis.png - Main performance comparison with anomaly highlights")
    print("2. variability_analysis.png - Statistical reliability analysis")
    print("3. anomaly_heatmap.png - Anomaly detection heatmap")
    print("4. theoretical_vs_actual.png - Theoretical vs actual performance comparison")
    print("\nAll plots saved with 300 DPI for publication quality.")
