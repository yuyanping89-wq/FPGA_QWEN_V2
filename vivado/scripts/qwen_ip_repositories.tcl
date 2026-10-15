# Explicit repositories prevent Vivado from discovering backup IP definitions.
# The optional delivered modules become active when copied into ip_repo.
set qwen_ip_repo_root [file normalize [file join [file dirname [info script]] .. ip_repo]]
set qwen_ip_repositories {}
foreach qwen_ip_name {m06_rmsnorm m09_kv_cache_update m13_swiglu m05_embedding m07_qkv_projection m11_attention_out m12_residual_add m14_lm_head_v2 m08_rope_v2 m10_attention_v2} {
  set qwen_ip_path [file join $qwen_ip_repo_root $qwen_ip_name]
  if {[file isfile [file join $qwen_ip_path component.xml]]} {
    lappend qwen_ip_repositories $qwen_ip_path
  } elseif {$qwen_ip_name in {m06_rmsnorm m09_kv_cache_update m13_swiglu}} {
    error "Required qwen IP repository missing: $qwen_ip_path"
  }
}
